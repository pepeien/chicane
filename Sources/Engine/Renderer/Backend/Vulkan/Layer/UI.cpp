#include "Chicane/Renderer/Backend/Vulkan/Layer/UI.hpp"

#include <algorithm>
#include <cstdint>

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Memory/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Sampler/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/View/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLUI::VulkanLUI()
            : Layer(u_LAYER_ID),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLUI::onInit()
        {
            buildGlyphBuffer();
            buildBackdrop();

            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildPrimitiveVertexBuffer();
            buildPrimitiveIndexBuffer();
        }

        void VulkanLUI::onRestart()
        {
            destroyBackdrop();
            buildBackdrop();

            VulkanBackend* backend = getBackend<VulkanBackend>();
            for (std::size_t i = 0; i < backend->frames.size(); i++)
            {
                vk::WriteDescriptorSet backdropInfo;
                backdropInfo.dstSet          = backend->frames.at(i).getDescriptorSet(m_id);
                backdropInfo.dstBinding      = 2;
                backdropInfo.dstArrayElement = 0;
                backdropInfo.descriptorCount = 1;
                backdropInfo.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                backdropInfo.pImageInfo      = &m_backdropInfos.at(i);

                backend->logicalDevice.updateDescriptorSets(backdropInfo, nullptr);
            }

            initFramebuffers();
        }

        void VulkanLUI::onDestruction()
        {
            destroyFrameResources();
            destroyPrimitiveData();
            destroyGlyphData();
            destroyBackdrop();

            m_graphicsPipeline.destroy();
        }

        void VulkanLUI::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e2D)
            {
                buildPrimitiveVertexData(inResource.getVertices());
                buildPrimitiveIndexData(inResource.getIndices());
                buildGlyphData(inResource.getGlyphOutlines());
            }
        }

        bool VulkanLUI::onBeginRender(const Frame& inFrame)
        {
            if (!inFrame.hasDraws(DrawPolyType::e2D, DrawPolyMode::Fill))
            {
                return false;
            }

            return true;
        }

        void VulkanLUI::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanFrame&      frame         = *((VulkanFrame*)inData);
            vk::CommandBuffer commandBuffer = frame.commandBuffer;

            std::size_t frameIndex = 0;
            for (; frameIndex < backend->frames.size(); frameIndex++)
            {
                if (&backend->frames.at(frameIndex) == &frame)
                {
                    break;
                }
            }

            vk::Viewport viewport = backend->getVkViewport(this);
            vk::Rect2D   scissor  = backend->getVkScissor(this);

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass               = m_graphicsPipeline.renderPass;
            beginInfo.framebuffer              = frame.image.getFramebuffer(m_id);
            beginInfo.renderArea.extent.width  = viewport.width;
            beginInfo.renderArea.extent.height = viewport.height;
            beginInfo.clearValueCount          = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues             = m_clear.data();

            bool inPass = false;

            auto endPass = [&]()
            {
                if (!inPass)
                {
                    return;
                }

                commandBuffer.endRenderPass();
                inPass = false;
            };

            auto beginPass = [&]()
            {
                if (inPass)
                {
                    return;
                }

                commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
                m_graphicsPipeline.bind(commandBuffer);
                m_graphicsPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));
                m_graphicsPipeline.bind(commandBuffer, 1, backend->textureDescriptor.set);

                const std::int32_t screenPush[4] = {backend->getScreenTextureId(), 0, 0, 0};
                commandBuffer.pushConstants(
                    m_graphicsPipeline.layout,
                    vk::ShaderStageFlagBits::eFragment,
                    0,
                    sizeof(screenPush),
                    screenPush
                );

                vk::Buffer     vertexBuffers[] = {m_primitiveVertexBuffer.instance};
                vk::DeviceSize offsets[]       = {0};
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.bindIndexBuffer(m_primitiveIndexBuffer.instance, 0, vk::IndexType::eUint32);
                commandBuffer.setViewport(0, 1, &viewport);
                commandBuffer.setScissor(0, 1, &scissor);

                inPass = true;
            };

            const DrawPoly2DInstance::List& instances = inFrame.getInstances2D();

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e2D, DrawPolyMode::Fill))
            {
                std::uint32_t       runStart    = draw.instanceStart;
                std::uint32_t       runCount    = 0;
                bool                runBackdrop = false;
                const std::uint32_t instanceEnd = draw.instanceStart + draw.instanceCount;

                auto flush = [&]()
                {
                    if (runCount == 0)
                    {
                        return;
                    }

                    if (runBackdrop)
                    {
                        endPass();
                        copyBackdrop(frame, frameIndex);
                    }

                    beginPass();
                    commandBuffer.drawIndexed(draw.indexCount, runCount, draw.indexStart, draw.vertexStart, runStart);
                    runCount = 0;
                };

                for (std::uint32_t i = draw.instanceStart; i < instanceEnd; i++)
                {
                    const bool backdrop = i < instances.size() && instances.at(i).backdropBlur > 0.0f;

                    if (runCount == 0)
                    {
                        runStart    = i;
                        runBackdrop = backdrop;
                        runCount    = 1;

                        continue;
                    }

                    if (backdrop != runBackdrop || backdrop)
                    {
                        flush();
                        runStart    = i;
                        runBackdrop = backdrop;
                        runCount    = 1;

                        continue;
                    }

                    runCount++;
                }

                flush();
            }

            endPass();
        }

        void VulkanLUI::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 3;

            // Primtive
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            // Glyph
            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            // Backdrop
            bidings.indices.push_back(2);
            bidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets * 2}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eCombinedImageSampler, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (std::size_t i = 0; i < backend->frames.size(); i++)
            {
                VulkanFrame&      frame = backend->frames.at(i);
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    backend->logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                frame.addDescriptorSet(m_id, descriptorSet);

                vk::WriteDescriptorSet primitiveInfo;
                primitiveInfo.dstSet          = descriptorSet;
                primitiveInfo.dstBinding      = 0;
                primitiveInfo.dstArrayElement = 0;
                primitiveInfo.descriptorCount = 1;
                primitiveInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                primitiveInfo.pBufferInfo     = &frame.poly2DResource.bufferInfo;
                frame.addWriteDescriptorSet(primitiveInfo);

                vk::WriteDescriptorSet glyphInfo;
                glyphInfo.dstSet          = descriptorSet;
                glyphInfo.dstBinding      = 1;
                glyphInfo.dstArrayElement = 0;
                glyphInfo.descriptorCount = 1;
                glyphInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                glyphInfo.pBufferInfo     = &m_glyphBufferInfo;
                frame.addWriteDescriptorSet(glyphInfo);

                vk::WriteDescriptorSet backdropInfo;
                backdropInfo.dstSet          = descriptorSet;
                backdropInfo.dstBinding      = 2;
                backdropInfo.dstArrayElement = 0;
                backdropInfo.descriptorCount = 1;
                backdropInfo.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                backdropInfo.pImageInfo      = &m_backdropInfos.at(i);
                frame.addWriteDescriptorSet(backdropInfo);
            }
        }

        void VulkanLUI::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLUI::initGraphicsPipeline()
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/UI.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Assets/Engine/Shaders/Vulkan/UI.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            // Depth
            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthBoundsTestEnable = false;
            depth.stencilTestEnable     = false;
            depth.depthWriteEnable      = false;
            depth.depthTestEnable       = false;
            depth.depthCompareOp        = vk::CompareOp::eAlways;
            depth.minDepthBounds        = 0.0f;
            depth.maxDepthBounds        = 1.0f;

            // Render pass
            vk::AttachmentDescription colorAttachment;
            colorAttachment.format        = backend->swapchain.colorFormat;
            colorAttachment.samples       = vk::SampleCountFlagBits::e1;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            colorAttachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachment.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;

            vk::AttachmentReference colorReference;
            colorReference.attachment = 0;
            colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDependency colorSubpassDepedency;
            colorSubpassDepedency.srcSubpass    = VK_SUBPASS_EXTERNAL;
            colorSubpassDepedency.dstSubpass    = 0;
            colorSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            colorSubpassDepedency.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

            vk::AttachmentDescription depthAttachment;
            depthAttachment.format        = backend->swapchain.depthFormat;
            depthAttachment.samples       = vk::SampleCountFlagBits::e1;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::AttachmentReference depthReference;
            depthReference.attachment = 1;
            depthReference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::SubpassDependency depthSubpassDepedency;
            depthSubpassDepedency.srcSubpass = 0;
            depthSubpassDepedency.dstSubpass = VK_SUBPASS_EXTERNAL;
            depthSubpassDepedency.srcStageMask =
                vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
            depthSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eEarlyFragmentTests;
            depthSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            depthSubpassDepedency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount    = 1;
            subpass.pColorAttachments       = &colorReference;
            subpass.pDepthStencilAttachment = &depthReference;

            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.depthClampEnable        = false;
            rasterization.depthBiasEnable         = false;
            rasterization.rasterizerDiscardEnable = false;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            vk::PushConstantRange screenPush;
            screenPush.stageFlags = vk::ShaderStageFlagBits::eFragment;
            screenPush.offset     = 0;
            screenPush.size       = sizeof(std::int32_t) * 4;

            // Build
            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addDynamicState(vk::DynamicState::eLineWidth)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState())
                .addAttachment(colorAttachment)
                .addSubpassDependecy(colorSubpassDepedency)
                .setDepthStencil(depth)
                .addAttachment(depthAttachment)
                .addSubpassDependecy(depthSubpassDepedency)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .addDescriptorSetLayout(backend->textureDescriptor.setLayout)
                .addPushConstant(screenPush)
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLUI::initFramebuffers()
        {
            VulkanBackend* backend  = getBackend<VulkanBackend>();
            vk::Viewport   viewport = backend->getVkViewport(this);

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                VulkanFrameBufferCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline.renderPass;
                createInfo.extent.width  = viewport.width;
                createInfo.extent.height = viewport.height;
                createInfo.attachments.push_back(image.colorImage.view);
                createInfo.attachments.push_back(image.depthImage.view);

                image.addBuffer(createInfo);
            }
        }

        void VulkanLUI::buildPrimitiveVertexBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = m_backend->getResourceBudget(Resource::UIVertices);
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_primitiveVertexBuffer.init(createInfo);
        }

        void VulkanLUI::buildPrimitiveVertexData(const Vertex::List& inVertices)
        {
            if (inVertices.empty())
            {
                return;
            }

            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = sizeof(Vertex) * inVertices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = backend->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inVertices.data(), createInfo.size);
            backend->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer
                .copy(m_primitiveVertexBuffer, createInfo.size, backend->graphicsQueue, backend->mainCommandBuffer);
            stagingBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLUI::buildPrimitiveIndexBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice   = backend->physicalDevice;
            createInfo.logicalDevice    = backend->logicalDevice;
            createInfo.size             = m_backend->getResourceBudget(Resource::UIIndices);
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_primitiveIndexBuffer.init(createInfo);
        }

        void VulkanLUI::buildPrimitiveIndexData(const Vertex::Indices& inIndices)
        {
            if (inIndices.empty())
            {
                return;
            }

            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = sizeof(Vertex::Index) * inIndices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = backend->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inIndices.data(), createInfo.size);
            backend->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer
                .copy(m_primitiveIndexBuffer, createInfo.size, backend->graphicsQueue, backend->mainCommandBuffer);
            stagingBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLUI::destroyPrimitiveData()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            m_primitiveVertexBuffer.destroy(backend->logicalDevice);
            m_primitiveIndexBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLUI::buildGlyphBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = m_backend->getResourceBudget(Resource::UIGlyphs);
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_glyphBuffer.init(createInfo);

            m_glyphBufferInfo.buffer = m_glyphBuffer.instance;
            m_glyphBufferInfo.offset = 0;
            m_glyphBufferInfo.range  = createInfo.size;
        }

        void VulkanLUI::buildGlyphData(const Outlines& inOutlines)
        {
            if (inOutlines.empty())
            {
                return;
            }

            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = sizeof(float) * inOutlines.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = backend->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inOutlines.data(), createInfo.size);
            backend->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(m_glyphBuffer, createInfo.size, backend->graphicsQueue, backend->mainCommandBuffer);
            stagingBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLUI::destroyGlyphData()
        {
            m_glyphBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }

        void VulkanLUI::buildBackdrop()
        {
            VulkanBackend*     backend = getBackend<VulkanBackend>();
            const vk::Extent2D extent  = backend->swapchain.extent;
            m_backdropMipLevels        = 1;
            {
                std::uint32_t size = std::max(extent.width, extent.height);
                while (size > 1)
                {
                    size >>= 1;
                    m_backdropMipLevels++;
                }
            }

            m_backdrops.resize(backend->frames.size());
            m_backdropInfos.resize(backend->frames.size());

            for (std::size_t i = 0; i < m_backdrops.size(); i++)
            {
                VulkanImageInfo& image = m_backdrops.at(i);
                image.format           = backend->swapchain.colorFormat;
                image.extent           = extent;

                VulkanImageCreateInfo instanceCreateInfo{};
                instanceCreateInfo.flags     = vk::ImageCreateFlags();
                instanceCreateInfo.width     = extent.width;
                instanceCreateInfo.height    = extent.height;
                instanceCreateInfo.count     = 1;
                instanceCreateInfo.mipLevels = m_backdropMipLevels;
                instanceCreateInfo.tiling    = vk::ImageTiling::eOptimal;
                instanceCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                                           vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format        = image.format;
                instanceCreateInfo.logicalDevice = backend->logicalDevice;
                VulkanImage::initInstance(image.instance, instanceCreateInfo);

                vk::SamplerCreateInfo samplerCreateInfo;
                samplerCreateInfo.minFilter               = vk::Filter::eLinear;
                samplerCreateInfo.magFilter               = vk::Filter::eLinear;
                samplerCreateInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
                samplerCreateInfo.addressModeU            = vk::SamplerAddressMode::eClampToEdge;
                samplerCreateInfo.addressModeV            = vk::SamplerAddressMode::eClampToEdge;
                samplerCreateInfo.addressModeW            = vk::SamplerAddressMode::eClampToEdge;
                samplerCreateInfo.anisotropyEnable        = false;
                samplerCreateInfo.maxAnisotropy           = 1.0f;
                samplerCreateInfo.borderColor             = vk::BorderColor::eFloatOpaqueBlack;
                samplerCreateInfo.unnormalizedCoordinates = false;
                samplerCreateInfo.compareEnable           = false;
                samplerCreateInfo.minLod                  = 0.0f;
                samplerCreateInfo.maxLod                  = static_cast<float>(std::max(1u, m_backdropMipLevels) - 1u);
                image.sampler                             = backend->logicalDevice.createSampler(samplerCreateInfo);

                VulkanImageMemoryCreateInfo memoryCreateInfo;
                memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice  = backend->logicalDevice;
                memoryCreateInfo.physicalDevice = backend->physicalDevice;
                VulkanImage::initMemory(image.memory, image.instance, memoryCreateInfo);

                VulkanImageViewCreateInfo viewCreateInfo;
                viewCreateInfo.count         = 1;
                viewCreateInfo.mipLevels     = m_backdropMipLevels;
                viewCreateInfo.type          = vk::ImageViewType::e2D;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
                viewCreateInfo.format        = image.format;
                viewCreateInfo.logicalDevice = backend->logicalDevice;
                VulkanImage::initView(image.view, image.instance, viewCreateInfo);

                VulkanImage::transitionLayout(
                    backend->mainCommandBuffer,
                    backend->graphicsQueue,
                    image.instance,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    1,
                    m_backdropMipLevels
                );

                vk::DescriptorImageInfo& info = m_backdropInfos.at(i);
                info.sampler                  = image.sampler;
                info.imageView                = image.view;
                info.imageLayout              = vk::ImageLayout::eShaderReadOnlyOptimal;
            }
        }

        void VulkanLUI::destroyBackdrop()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            for (VulkanImageInfo& image : m_backdrops)
            {
                if (image.sampler)
                {
                    backend->logicalDevice.destroySampler(image.sampler);
                }

                if (image.view)
                {
                    backend->logicalDevice.destroyImageView(image.view);
                }

                if (image.instance)
                {
                    backend->logicalDevice.destroyImage(image.instance);
                }

                if (image.memory)
                {
                    backend->logicalDevice.freeMemory(image.memory);
                }
            }

            m_backdrops.clear();
            m_backdropInfos.clear();
        }

        void VulkanLUI::copyBackdrop(VulkanFrame& inFrame, std::size_t inIndex)
        {
            if (inIndex >= m_backdrops.size())
            {
                return;
            }

            const VulkanImageInfo& backdrop = m_backdrops.at(inIndex);
            const vk::Image        source   = inFrame.image.targetImage.instance;
            vk::CommandBuffer      commands = inFrame.commandBuffer;
            const vk::Extent2D     extent   = backdrop.extent;

            vk::ImageSubresourceRange range;
            range.aspectMask     = vk::ImageAspectFlagBits::eColor;
            range.baseMipLevel   = 0;
            range.levelCount     = 1;
            range.baseArrayLayer = 0;
            range.layerCount     = 1;

            vk::ImageMemoryBarrier sourceToTransfer;
            sourceToTransfer.oldLayout           = vk::ImageLayout::eShaderReadOnlyOptimal;
            sourceToTransfer.newLayout           = vk::ImageLayout::eTransferSrcOptimal;
            sourceToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            sourceToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            sourceToTransfer.image               = source;
            sourceToTransfer.srcAccessMask       = vk::AccessFlagBits::eShaderRead;
            sourceToTransfer.dstAccessMask       = vk::AccessFlagBits::eTransferRead;
            sourceToTransfer.subresourceRange    = range;

            vk::ImageMemoryBarrier backdropToTransfer;
            backdropToTransfer.oldLayout           = vk::ImageLayout::eShaderReadOnlyOptimal;
            backdropToTransfer.newLayout           = vk::ImageLayout::eTransferDstOptimal;
            backdropToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            backdropToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            backdropToTransfer.image               = backdrop.instance;
            backdropToTransfer.srcAccessMask       = vk::AccessFlagBits::eShaderRead;
            backdropToTransfer.dstAccessMask       = vk::AccessFlagBits::eTransferWrite;
            backdropToTransfer.subresourceRange    = range;

            std::array<vk::ImageMemoryBarrier, 2> before = {sourceToTransfer, backdropToTransfer};
            commands.pipelineBarrier(
                vk::PipelineStageFlagBits::eFragmentShader,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                before
            );

            vk::ImageCopy region;
            region.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
            region.srcSubresource.mipLevel       = 0;
            region.srcSubresource.baseArrayLayer = 0;
            region.srcSubresource.layerCount     = 1;
            region.dstSubresource                = region.srcSubresource;
            region.extent                        = vk::Extent3D(extent.width, extent.height, 1);

            commands.copyImage(
                source,
                vk::ImageLayout::eTransferSrcOptimal,
                backdrop.instance,
                vk::ImageLayout::eTransferDstOptimal,
                region
            );

            generateBackdropMips(commands, inIndex);

            vk::ImageMemoryBarrier sourceToSample = sourceToTransfer;
            sourceToSample.oldLayout              = vk::ImageLayout::eTransferSrcOptimal;
            sourceToSample.newLayout              = vk::ImageLayout::eShaderReadOnlyOptimal;
            sourceToSample.srcAccessMask          = vk::AccessFlagBits::eTransferRead;
            sourceToSample.dstAccessMask          = vk::AccessFlagBits::eShaderRead;

            commands.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                sourceToSample
            );
        }

        void VulkanLUI::generateBackdropMips(vk::CommandBuffer inCommands, std::size_t inIndex)
        {
            const VulkanImageInfo& backdrop = m_backdrops.at(inIndex);
            const std::uint32_t    levels   = std::max(1u, m_backdropMipLevels);

            vk::ImageSubresourceRange range;
            range.aspectMask     = vk::ImageAspectFlagBits::eColor;
            range.baseMipLevel   = 0;
            range.levelCount     = 1;
            range.baseArrayLayer = 0;
            range.layerCount     = 1;

            auto barrier = [&](std::uint32_t          inLevel,
                               vk::ImageLayout        inOldLayout,
                               vk::ImageLayout        inNewLayout,
                               vk::AccessFlags        inSrcAccess,
                               vk::AccessFlags        inDstAccess,
                               vk::PipelineStageFlags inSrcStage,
                               vk::PipelineStageFlags inDstStage)
            {
                vk::ImageMemoryBarrier imageBarrier;
                imageBarrier.oldLayout                     = inOldLayout;
                imageBarrier.newLayout                     = inNewLayout;
                imageBarrier.srcQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
                imageBarrier.dstQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
                imageBarrier.image                         = backdrop.instance;
                imageBarrier.srcAccessMask                 = inSrcAccess;
                imageBarrier.dstAccessMask                 = inDstAccess;
                imageBarrier.subresourceRange              = range;
                imageBarrier.subresourceRange.baseMipLevel = inLevel;

                inCommands
                    .pipelineBarrier(inSrcStage, inDstStage, vk::DependencyFlags(), nullptr, nullptr, imageBarrier);
            };

            if (levels == 1)
            {
                barrier(
                    0,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eTransferWrite,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader
                );

                return;
            }

            barrier(
                0,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eTransferSrcOptimal,
                vk::AccessFlagBits::eTransferWrite,
                vk::AccessFlagBits::eTransferRead,
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eTransfer
            );

            std::int32_t srcWidth  = static_cast<std::int32_t>(backdrop.extent.width);
            std::int32_t srcHeight = static_cast<std::int32_t>(backdrop.extent.height);

            for (std::uint32_t level = 1; level < levels; level++)
            {
                const std::int32_t dstWidth  = std::max(1, srcWidth / 2);
                const std::int32_t dstHeight = std::max(1, srcHeight / 2);

                barrier(
                    level,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::AccessFlagBits::eNone,
                    vk::AccessFlagBits::eTransferWrite,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eTransfer
                );

                vk::ImageBlit blit{};
                blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
                blit.srcSubresource.mipLevel       = level - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount     = 1;
                blit.srcOffsets[1]                 = vk::Offset3D(srcWidth, srcHeight, 1);
                blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
                blit.dstSubresource.mipLevel       = level;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount     = 1;
                blit.dstOffsets[1]                 = vk::Offset3D(dstWidth, dstHeight, 1);

                inCommands.blitImage(
                    backdrop.instance,
                    vk::ImageLayout::eTransferSrcOptimal,
                    backdrop.instance,
                    vk::ImageLayout::eTransferDstOptimal,
                    blit,
                    vk::Filter::eLinear
                );

                barrier(
                    level - 1,
                    vk::ImageLayout::eTransferSrcOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eTransferRead,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader
                );

                if (level + 1 < levels)
                {
                    barrier(
                        level,
                        vk::ImageLayout::eTransferDstOptimal,
                        vk::ImageLayout::eTransferSrcOptimal,
                        vk::AccessFlagBits::eTransferWrite,
                        vk::AccessFlagBits::eTransferRead,
                        vk::PipelineStageFlagBits::eTransfer,
                        vk::PipelineStageFlagBits::eTransfer
                    );
                }
                else
                {
                    barrier(
                        level,
                        vk::ImageLayout::eTransferDstOptimal,
                        vk::ImageLayout::eShaderReadOnlyOptimal,
                        vk::AccessFlagBits::eTransferWrite,
                        vk::AccessFlagBits::eShaderRead,
                        vk::PipelineStageFlagBits::eTransfer,
                        vk::PipelineStageFlagBits::eFragmentShader
                    );
                }

                srcWidth  = dstWidth;
                srcHeight = dstHeight;
            }
        }
    }
}