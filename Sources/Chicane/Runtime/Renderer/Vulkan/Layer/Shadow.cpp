#include "Chicane/Runtime/Renderer/Vulkan/Layer/Shadow.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LShadow::LShadow()
            : Layer::Instance("Shadow"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_clearValues({}),
            m_modelManager(Box::getModelManager())
        {
            m_clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

            loadEvents();
        }

        LShadow::~LShadow()
        {
            if (is(Layer::Status::Offline))
            {
                return;
            }

            m_internals.logicalDevice.waitIdle();

            destroyModelData();

            m_graphicsPipeline.reset();
        }

        void LShadow::build()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildModelData();

            setStatus(Layer::Status::Running);
        }

        void LShadow::destroy()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            destroyFrameResources();

            setStatus(Layer::Status::Idle);
        }

        void LShadow::rebuild()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            initFrameResources();
            initFramebuffers();

            setStatus(Layer::Status::Running);
        }

        void LShadow::render(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Renderer::Data* data             = (Renderer::Data*) outData;
            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Frame::Instance& frame           = data->frame;

            vk::RenderPassBeginInfo beginInfo = {};
            beginInfo.renderPass          = m_graphicsPipeline->renderPass;
            beginInfo.framebuffer         = frame.getFramebuffer(m_id);
            beginInfo.renderArea.offset.x = 0;
            beginInfo.renderArea.offset.y = 0;
            beginInfo.renderArea.extent   = data->swapChainExtent;
            beginInfo.clearValueCount     = static_cast<std::uint32_t>(m_clearValues.size());
            beginInfo.pClearValues        = m_clearValues.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
                // Pipeline
                m_graphicsPipeline->bind(commandBuffer);

                // Frame
                m_graphicsPipeline->bindDescriptorSet(commandBuffer, 0, frame.getDescriptorSet(m_id));

                // Model
                renderModels(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void LShadow::loadEvents()
        {
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            setStatus(Layer::Status::Idle);

            m_modelManager->watchChanges(
                [&](Box::Manager::EventType inEvent)
                {
                    if (inEvent != Box::Manager::EventType::Activation)
                    {
                        return;
                    }

                    if (is(Layer::Status::Idle))
                    {
                        setStatus(Layer::Status::Initialized);

                        build();

                        return;
                    }

                    if (is(Layer::Status::Running))
                    {
                        rebuildModelData();
                    }
                }
            );
        }

        void LShadow::initFrameResources()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo bidings = {};
            bidings.count = 2;

            /// Light
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            /// Model
            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                bidings
            );

            Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets  = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = descriptorPoolCreateInfo.maxSets }
            );
            descriptorPoolCreateInfo.sizes.push_back(
                { .type = vk::DescriptorType::eStorageBuffer, .descriptorCount = descriptorPoolCreateInfo.maxSets }
            );

            Descriptor::initPool(
                m_frameDescriptor.pool,
                m_internals.logicalDevice,
                descriptorPoolCreateInfo
            );

            for (Frame::Instance& frame : m_internals.swapchain->frames)
            {
                vk::DescriptorSet descriptorSet;

                Descriptor::allocalteSet(
                    descriptorSet,
                    m_internals.logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                frame.addDescriptorSet(m_id, descriptorSet);

                vk::WriteDescriptorSet lightWriteInfo = {};
                lightWriteInfo.dstSet          = descriptorSet;
                lightWriteInfo.dstBinding      = 0;
                lightWriteInfo.dstArrayElement = 0;
                lightWriteInfo.descriptorCount = 1;
                lightWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                lightWriteInfo.pBufferInfo     = &frame.lightResource.bufferInfo;
                frame.addWriteDescriptorSet(lightWriteInfo);

                vk::WriteDescriptorSet modelWriteInfo = {};
                modelWriteInfo.dstSet          = descriptorSet;
                modelWriteInfo.dstBinding      = 1;
                modelWriteInfo.dstArrayElement = 0;
                modelWriteInfo.descriptorCount = 1;
                modelWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                modelWriteInfo.pBufferInfo     = &frame.meshResource.bufferInfo;
                frame.addWriteDescriptorSet(modelWriteInfo);
            }
        }

        void LShadow::destroyFrameResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );
            m_internals.logicalDevice.destroyDescriptorPool(
                m_frameDescriptor.pool
            );
        }

        void LShadow::initGraphicsPipeline()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterizeCreateInfo = {};
            rasterizeCreateInfo.flags                   = vk::PipelineRasterizationStateCreateFlags();
            rasterizeCreateInfo.depthClampEnable        = VK_FALSE;
            rasterizeCreateInfo.rasterizerDiscardEnable = VK_FALSE;
            rasterizeCreateInfo.depthBiasEnable         = VK_TRUE;
            rasterizeCreateInfo.depthBiasClamp          = 0.0f;
            rasterizeCreateInfo.polygonMode             = vk::PolygonMode::eFill;
            rasterizeCreateInfo.cullMode                = vk::CullModeFlagBits::eBack;
            rasterizeCreateInfo.frontFace               = vk::FrontFace::eClockwise;
            rasterizeCreateInfo.lineWidth               = 1.0f;
            rasterizeCreateInfo.depthBiasConstantFactor = 1.25f;
            rasterizeCreateInfo.depthBiasSlopeFactor    = 1.75f;

            // Shader
            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Contents/Chicane/Runtime/Vulkan/Shaders/shadow.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            std::vector<Shader::StageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);

            // Set Layouts
            std::vector<vk::DescriptorSetLayout> setLayouts = {};
            setLayouts.push_back(m_frameDescriptor.setLayout);

            // Attachments
            GraphicsPipeline::Attachment depthAttachment = {};
            depthAttachment.type          = GraphicsPipeline::Attachment::Type::Depth;
            depthAttachment.format        = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;

            std::vector<GraphicsPipeline::Attachment> attachments = {};
            attachments.push_back(depthAttachment);

            GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.bHasVertices             = true;
            createInfo.bHasDepthWrite           = true;
            createInfo.bHasBlending             = false;
            createInfo.logicalDevice            = m_internals.logicalDevice;
            createInfo.shaders                  = shaders;
            createInfo.extent                   = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts     = setLayouts;
            createInfo.attachments              = attachments;
            createInfo.rasterizaterizationState = rasterizeCreateInfo;

            m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
        }

        void LShadow::initFramebuffers()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            for (Frame::Instance& frame : m_internals.swapchain->frames)
            {
                Frame::Buffer::CreateInfo createInfo = {};
                createInfo.id              = m_id;
                createInfo.logicalDevice   = m_internals.logicalDevice;
                createInfo.renderPass      = m_graphicsPipeline->renderPass;
                createInfo.extent          = m_internals.swapchain->extent;
                createInfo.attachments.push_back(frame.shadowImage.view);

                Frame::Buffer::init(frame, createInfo);
            }
        }

        void LShadow::buildModelVertexBuffer()
        {
            const auto& vertices = m_modelManager->getVertices();

            Buffer::CreateInfo createInfo = {};
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Box::Model::Vertex) * vertices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                createInfo.size
            );
            memcpy(writeLocation, vertices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Buffer::init(m_modelVertexBuffer, createInfo);
            Buffer::copy(
                stagingBuffer,
                m_modelVertexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Buffer::destroy(m_internals.logicalDevice, stagingBuffer);
        }

        void LShadow::buildModelIndexBuffer()
        {
            const auto& indices = m_modelManager->getIndices();

            Buffer::CreateInfo createInfo;
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(std::uint32_t) * indices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                createInfo.size
            );
            memcpy(writeLocation, indices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Buffer::init(m_modelIndexBuffer, createInfo);
            Buffer::copy(
                stagingBuffer,
                m_modelIndexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Buffer::destroy(m_internals.logicalDevice, stagingBuffer);
        }

        void LShadow::buildModelData()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LShadow::destroyModelData()
        {
            m_internals.logicalDevice.waitIdle();

            Buffer::destroy(
                m_internals.logicalDevice,
                m_modelVertexBuffer
            );
            Buffer::destroy(
                m_internals.logicalDevice,
                m_modelIndexBuffer
            );
        }

        void LShadow::rebuildModelData()
        {
            destroyModelData();

            if (m_modelManager->isEmpty())
            {
                return;
            }

            buildModelData();
        }

        void LShadow::renderModels(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Buffer vertexBuffers[] = { m_modelVertexBuffer.instance };
            vk::DeviceSize offsets[]   = { 0 };

            inCommandBuffer.bindVertexBuffers(
                0,
                1,
                vertexBuffers,
                offsets
            );

            inCommandBuffer.bindIndexBuffer(
                m_modelIndexBuffer.instance,
                0,
                vk::IndexType::eUint32
            );

            for (const std::string& id : m_modelManager->getActiveIds())
            {
                const auto& data = m_modelManager->getData(id);

                inCommandBuffer.drawIndexed(
                    data.indexCount,
                    m_modelManager->getUseCount(id),
                    data.firstIndex,
                    0,
                    m_modelManager->getFirstUse(id)
                );
            }
        }
    }
}