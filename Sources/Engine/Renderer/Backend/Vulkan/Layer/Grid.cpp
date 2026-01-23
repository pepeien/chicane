#include "Chicane/Renderer/Backend/Vulkan/Layer/Grid.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLGrid::VulkanLGrid()
            : Layer("Engine_Grid"),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        VulkanLGrid::~VulkanLGrid()
        {
            deleteChildren();
            destroyFrameResources();
            destroyPrimitiveData();

            m_graphicsPipeline.reset();
        }

        bool VulkanLGrid::onInit()
        {
            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildPrimitiveVertexBuffer();
            buildPrimitiveIndexBuffer();

            return true;
        }

        void VulkanLGrid::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e2D)
            {
                buildPrimitiveVertexData(inResource.getVertices());
                buildPrimitiveIndexData(inResource.getIndices());
            }
        }

        bool VulkanLGrid::onRebuild()
        {
            initFrameResources();
            initFramebuffers();

            return true;
        }

        bool VulkanLGrid::onSetup(const Frame& inFrame)
        {
            if (inFrame.getInstances2D().empty())
            {
                return false;
            }

            if (inFrame.get2DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void VulkanLGrid::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackendData* data          = (VulkanBackendData*)inData;
            VulkanFrame        frame         = data->frame;
            vk::CommandBuffer  commandBuffer = data->commandBuffer;

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass        = m_graphicsPipeline->renderPass;
            beginInfo.framebuffer       = frame.getFramebuffer(m_id);
            beginInfo.renderArea.extent = getBackend<VulkanBackend>()->swapchain.extent;
            beginInfo.clearValueCount   = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues      = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

            // Pipeline
            m_graphicsPipeline->bind(commandBuffer);

            // Frame
            m_graphicsPipeline->bindDescriptorSet(commandBuffer, 0, frame.getDescriptorSet(m_id));

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

        bool VulkanLGrid::onDestroy()
        {
            destroyFrameResources();

            return true;
        }

        void VulkanLGrid::initFrameResources()
        {
            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 1;

            // Primtive
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(
                m_frameDescriptor.setLayout,
                getBackend<VulkanBackend>()->logicalDevice,
                bidings
            );

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets =
                static_cast<std::uint32_t>(getBackend<VulkanBackend>()->swapchain.frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(
                m_frameDescriptor.pool,
                getBackend<VulkanBackend>()->logicalDevice,
                descriptorPoolCreateInfo
            );

            for (VulkanFrame& frame : getBackend<VulkanBackend>()->swapchain.frames)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    getBackend<VulkanBackend>()->logicalDevice,
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
            }
        }

        void VulkanLGrid::destroyFrameResources()
        {
            getBackend<VulkanBackend>()->logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );
            getBackend<VulkanBackend>()->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLGrid::initGraphicsPipeline()
        {
            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Grid.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Grid.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<VulkanShaderStageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);
            shaders.push_back(fragmentShader);

            // Attachments
            VulkanGraphicsPipelineAttachment colorAttachment;
            colorAttachment.type          = VulkanGraphicsPipelineAttachmentType::Color;
            colorAttachment.format        = getBackend<VulkanBackend>()->swapchain.colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            VulkanGraphicsPipelineAttachment depthAttachment;
            depthAttachment.type          = VulkanGraphicsPipelineAttachmentType::Depth;
            depthAttachment.format        = getBackend<VulkanBackend>()->swapchain.depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            std::vector<VulkanGraphicsPipelineAttachment> attachments = {};
            attachments.push_back(colorAttachment);
            attachments.push_back(depthAttachment);

            // Set Layouts
            std::vector<vk::DescriptorSetLayout> setLayouts = {};
            setLayouts.push_back(m_frameDescriptor.setLayout);

            VulkanGraphicsPipelineCreateInfo createInfo;
            createInfo.bHasVertices         = true;
            createInfo.bHasDepthTest        = true;
            createInfo.bHasDepthWrite       = false;
            createInfo.bHasBlending         = true;
            createInfo.logicalDevice        = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.shaders              = shaders;
            createInfo.extent               = getBackend<VulkanBackend>()->swapchain.extent;
            createInfo.descriptorSetLayouts = setLayouts;
            createInfo.attachments          = attachments;
            createInfo.rasterizaterizationState =
                VulkanGraphicsPipeline::createRasterizationState(vk::PolygonMode::eFill);

            m_graphicsPipeline = std::make_unique<VulkanGraphicsPipeline>(createInfo);
        }

        void VulkanLGrid::initFramebuffers()
        {
            for (VulkanFrame& frame : getBackend<VulkanBackend>()->swapchain.frames)
            {
                VulkanFrameCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = getBackend<VulkanBackend>()->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline->renderPass;
                createInfo.extent        = getBackend<VulkanBackend>()->swapchain.extent;
                createInfo.attachments.push_back(frame.colorImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                frame.addBuffer(createInfo);
            }
        }

        void VulkanLGrid::buildPrimitiveVertexBuffer()
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex) * 2000000;
            createInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_primitiveVertexBuffer.init(createInfo);
        }

        void VulkanLGrid::buildPrimitiveVertexData(const Vertex::List& inVertices)
        {
            if (inVertices.empty())
            {
                return;
            }

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex) * inVertices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = getBackend<VulkanBackend>()
                                      ->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inVertices.data(), createInfo.size);
            getBackend<VulkanBackend>()->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(
                m_primitiveVertexBuffer,
                createInfo.size,
                getBackend<VulkanBackend>()->graphicsQueue,
                getBackend<VulkanBackend>()->mainCommandBuffer
            );
            stagingBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }

        void VulkanLGrid::buildPrimitiveIndexBuffer()
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex::Index) * 2000000;
            createInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_primitiveIndexBuffer.init(createInfo);
        }

        void VulkanLGrid::buildPrimitiveIndexData(const Vertex::Indices& inIndices)
        {
            if (inIndices.empty())
            {
                return;
            }

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex::Index) * inIndices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = getBackend<VulkanBackend>()
                                      ->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inIndices.data(), createInfo.size);
            getBackend<VulkanBackend>()->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(
                m_primitiveIndexBuffer,
                createInfo.size,
                getBackend<VulkanBackend>()->graphicsQueue,
                getBackend<VulkanBackend>()->mainCommandBuffer
            );
            stagingBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }

        void VulkanLGrid::destroyPrimitiveData()
        {
            getBackend<VulkanBackend>()->logicalDevice.waitIdle();

            m_primitiveVertexBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
            m_primitiveIndexBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }
    }
}