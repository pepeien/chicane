#include "Chicane/Renderer/Backend/Vulkan/Layer/Grid.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLGrid::VulkanLGrid()
            : Layer(ID),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLGrid::onInit()
        {
            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildPrimitiveVertexBuffer();
            buildPrimitiveIndexBuffer();
        }

        void VulkanLGrid::onRestart()
        {
            initFrameResources();
            initFramebuffers();
        }

        void VulkanLGrid::onShutdown()
        {
            destroyFrameResources();
        }

        void VulkanLGrid::onDestruction()
        {
            destroyPrimitiveData();

            m_graphicsPipeline.destroy();
        }

        void VulkanLGrid::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e2D)
            {
                buildPrimitiveVertexData(inResource.getVertices());
                buildPrimitiveIndexData(inResource.getIndices());
            }
        }

        bool VulkanLGrid::onBeginRender(const Frame& inFrame)
        {
            if (inFrame.getInstances2D().empty() || inFrame.get2DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void VulkanLGrid::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanSwapchainImage image         = *((VulkanSwapchainImage*)inData);
            vk::CommandBuffer    commandBuffer = image.commandBuffer;

            vk::Viewport viewport = backend->getVkViewport(this);
            commandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor = backend->getVkScissor(this);
            commandBuffer.setScissor(0, 1, &scissor);

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass               = m_graphicsPipeline.renderPass;
            beginInfo.framebuffer              = image.getFramebuffer(m_id);
            beginInfo.renderArea.extent.width  = viewport.width;
            beginInfo.renderArea.extent.height = viewport.height;
            beginInfo.clearValueCount          = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues             = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

            // Pipeline
            m_graphicsPipeline.bind(commandBuffer);

            // Frame
            m_graphicsPipeline.bind(commandBuffer, 0, image.getDescriptorSet(m_id));

            // Texture
            m_graphicsPipeline.bind(commandBuffer, 1, backend->textureDescriptor.set);

            // Draw
            vk::Buffer     vertexBuffers[] = {m_primitiveVertexBuffer.instance};
            vk::DeviceSize offsets[]       = {0};

            commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            commandBuffer.bindIndexBuffer(m_primitiveIndexBuffer.instance, 0, vk::IndexType::eUint32);

            for (const DrawPoly& draw : inFrame.get2DDraws())
            {
                if (draw.instanceCount == 0)
                {
                    continue;
                }

                commandBuffer.drawIndexed(
                    draw.indexCount,
                    draw.instanceCount,
                    draw.indexStart,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }

            commandBuffer.endRenderPass();
        }

        void VulkanLGrid::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 1;

            // Primtive
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->swapchain.images.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    backend->logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                image.addDescriptorSet(m_id, descriptorSet);

                vk::WriteDescriptorSet primitiveInfo;
                primitiveInfo.dstSet          = descriptorSet;
                primitiveInfo.dstBinding      = 0;
                primitiveInfo.dstArrayElement = 0;
                primitiveInfo.descriptorCount = 1;
                primitiveInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                primitiveInfo.pBufferInfo     = &image.poly2DResource.bufferInfo;
                image.addWriteDescriptorSet(primitiveInfo);
            }
        }

        void VulkanLGrid::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLGrid::initGraphicsPipeline()
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Grid.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Grid.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            // Depth
            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.flags                 = vk::PipelineDepthStencilStateCreateFlags();
            depth.depthBoundsTestEnable = VK_FALSE;
            depth.stencilTestEnable     = VK_FALSE;
            depth.depthWriteEnable      = VK_TRUE;
            depth.depthTestEnable       = VK_TRUE;
            depth.depthCompareOp        = vk::CompareOp::eLessOrEqual;
            depth.minDepthBounds        = 0.0f;
            depth.maxDepthBounds        = 1.0f;

            // Render pass
            vk::AttachmentDescription colorAttachment;
            colorAttachment.flags         = vk::AttachmentDescriptionFlags();
            colorAttachment.format        = backend->swapchain.colorFormat;
            colorAttachment.samples       = vk::SampleCountFlagBits::e1;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

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
            depthAttachment.flags         = vk::AttachmentDescriptionFlags();
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
            subpass.flags                   = vk::SubpassDescriptionFlags();
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount    = 1;
            subpass.pColorAttachments       = &colorReference;
            subpass.pDepthStencilAttachment = &depthReference;

            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.flags                   = vk::PipelineRasterizationStateCreateFlags();
            rasterization.depthClampEnable        = VK_FALSE;
            rasterization.depthBiasEnable         = VK_FALSE;
            rasterization.rasterizerDiscardEnable = VK_FALSE;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eBack;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;
            rasterization.lineWidth               = 1.0f;
            rasterization.depthBiasConstantFactor = 0.0f;
            rasterization.depthBiasClamp          = 0.0f;
            rasterization.depthBiasSlopeFactor    = 0.0f;

            // Build
            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
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
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLGrid::initFramebuffers()
        {
            VulkanBackend* backend  = getBackend<VulkanBackend>();
            vk::Viewport   viewport = backend->getVkViewport(this);

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                VulkanFrameCreateInfo createInfo;
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

        void VulkanLGrid::buildPrimitiveVertexBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = m_backend->getResourceBudget(BackendResource::UIVertices);
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_primitiveVertexBuffer.init(createInfo);
        }

        void VulkanLGrid::buildPrimitiveVertexData(const Vertex::List& inVertices)
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

        void VulkanLGrid::buildPrimitiveIndexBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice   = backend->physicalDevice;
            createInfo.logicalDevice    = backend->logicalDevice;
            createInfo.size             = m_backend->getResourceBudget(BackendResource::UIIndices);
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_primitiveIndexBuffer.init(createInfo);
        }

        void VulkanLGrid::buildPrimitiveIndexData(const Vertex::Indices& inIndices)
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

        void VulkanLGrid::destroyPrimitiveData()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            m_primitiveVertexBuffer.destroy(backend->logicalDevice);
            m_primitiveIndexBuffer.destroy(backend->logicalDevice);
        }
    }
}