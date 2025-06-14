#include "Layer/Grid.hpp"

namespace Chicane
{
    namespace Editor
    {
        LGrid::LGrid()
            : Layer::Instance("Editor"),
            m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
            m_clearValues.push_back(vk::ClearDepthStencilValue(0.0f, 0));

            Box::getModelManager()->watchChanges(
                [this](Box::Manager::EventType inEvent)
                {
                    if (inEvent != Box::Manager::EventType::Activation || !is(Layer::Status::Offline))
                    {
                        return;
                    }

                    setStatus(Layer::Status::Initialized);

                    build();
                }
            );
        }

        LGrid::~LGrid()
        {
            m_internals.logicalDevice.waitIdle();

            // Graphics Pipeline
            m_graphicsPipeline.reset();

            // Descriptors
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );

            // Buffers
            Vulkan::Buffer::destroy(
                m_internals.logicalDevice,
                m_vertexBuffer
            );
        }

        void LGrid::build()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            initFrameDescriptorSetLayout();
            initGraphicsPipeline();
            initFramebuffers();
            initFrameResources();
            initVertexBuffers();

            setStatus(Layer::Status::Running);
        }

        void LGrid::destroy()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            m_internals.logicalDevice.waitIdle();

            m_internals.logicalDevice.destroyDescriptorPool(
                m_frameDescriptor.pool
            );

            setStatus(Layer::Status::Offline);
        }

        void LGrid::rebuild()
        {
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            initFramebuffers();
            initFrameResources();

            setStatus(Layer::Status::Running);
        }

        void LGrid::render(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Vulkan::Renderer::Data* data = (Vulkan::Renderer::Data*) outData;

            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Vulkan::Frame::Instance& frame = data->frame;

            vk::RenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
            renderPassBeginInfo.framebuffer         = frame.getFramebuffer(m_id);
            renderPassBeginInfo.renderArea.offset.x = 0;
            renderPassBeginInfo.renderArea.offset.y = 0;
            renderPassBeginInfo.renderArea.extent   = data->swapChainExtent;
            renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(m_clearValues.size());
            renderPassBeginInfo.pClearValues        = m_clearValues.data();

            commandBuffer.beginRenderPass(
                &renderPassBeginInfo,
                vk::SubpassContents::eInline
            );
                commandBuffer.bindPipeline(
                    vk::PipelineBindPoint::eGraphics,
                    m_graphicsPipeline->instance
                );
                commandBuffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    m_graphicsPipeline->layout,
                    0,
                    frame.getDescriptorSet(m_id),
                    nullptr
                );

                vk::Buffer vertexBuffers[] = { m_vertexBuffer.instance };
                vk::DeviceSize offsets[]   = { 0 };

                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.draw(6, 1, 0, 0);
            commandBuffer.endRenderPass();
        }

        void LGrid::initFrameDescriptorSetLayout()
        {
            Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 1;
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

            Vulkan::Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                frameLayoutBidings
            );
        }

        std::vector<Vulkan::Shader::StageCreateInfo> LGrid::getGraphicsPipelineShaders()
        {
            Vulkan::Shader::StageCreateInfo vertexShader {};
            vertexShader.path = "Content/Shaders/grid.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Vulkan::Shader::StageCreateInfo fragmentShader {};
            fragmentShader.path = "Content/Shaders/grid.frag.spv";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Vulkan::Shader::StageCreateInfo> result {};
            result.push_back(vertexShader);
            result.push_back(fragmentShader);

            return result;    
        }

        std::vector<vk::DescriptorSetLayout> LGrid::getGraphicsPipelineDescriptorLayouts()
        {
            std::vector<vk::DescriptorSetLayout> result {};
            result.push_back(m_frameDescriptor.setLayout);

            return result;
        }

        std::vector<Vulkan::GraphicsPipeline::Attachment> LGrid::getGraphicsPipelineAttachments()
        {
            // Attachments
            Vulkan::GraphicsPipeline::Attachment colorAttachment = {};
            colorAttachment.type          = Vulkan::GraphicsPipeline::Attachment::Type::Color;
            colorAttachment.format        = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            Vulkan::GraphicsPipeline::Attachment depthAttachment = {};
            depthAttachment.type          = Vulkan::GraphicsPipeline::Attachment::Type::Depth;
            depthAttachment.format        = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            std::vector<Vulkan::GraphicsPipeline::Attachment> result = {};
            result.push_back(colorAttachment);
            result.push_back(depthAttachment);

            return result;
        }

        void LGrid::initGraphicsPipeline()
        {
            Vulkan::GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.bHasVertices             = false;
            createInfo.bHasDepthWrite           = false;
            createInfo.bHasBlending             = true;
            createInfo.logicalDevice            = m_internals.logicalDevice;
            createInfo.shaders                  = getGraphicsPipelineShaders();
            createInfo.extent                   = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts     = getGraphicsPipelineDescriptorLayouts();
            createInfo.attachments              = getGraphicsPipelineAttachments();
            createInfo.rasterizaterizationState = Vulkan::GraphicsPipeline::createRasterizationState(vk::PolygonMode::eFill);

            m_graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline::Instance>(createInfo);
        }

        void LGrid::initFramebuffers()
        {
            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                Vulkan::Frame::Buffer::CreateInfo createInfo = {};
                createInfo.id            = m_id;
                createInfo.logicalDevice = m_internals.logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline->renderPass;
                createInfo.extent        = m_internals.swapchain->extent;
                createInfo.attachments.push_back(frame.colorImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                Vulkan::Frame::Buffer::init(frame, createInfo);
            }
        }

        void LGrid::initFrameResources()
        {
            Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_internals.swapchain->frames.size());
            descriptorPoolCreateInfo.sizes = {
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = descriptorPoolCreateInfo.maxSets }
            };

            Vulkan::Descriptor::initPool(
                m_frameDescriptor.pool,
                m_internals.logicalDevice,
                descriptorPoolCreateInfo
            );

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                // Scene
                vk::DescriptorSet sceneDescriptorSet;
                Vulkan::Descriptor::allocalteSet(
                    sceneDescriptorSet,
                    m_internals.logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                frame.addDescriptorSet(m_id, sceneDescriptorSet);

                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = sceneDescriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);
            }
        }

        void LGrid::initVertexBuffers()
        {
            std::vector<float> vertices = {
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f
            };

            Vulkan::Buffer::CreateInfo stagingBufferCreateInfo;
            stagingBufferCreateInfo.physicalDevice   = m_internals.physicalDevice;
            stagingBufferCreateInfo.logicalDevice    = m_internals.logicalDevice;
            stagingBufferCreateInfo.size             = sizeof(float) * vertices.size();
            stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                       vk::MemoryPropertyFlagBits::eHostCoherent;

            Vulkan::Buffer::Instance stagingBuffer;
            Vulkan::Buffer::init(stagingBuffer, stagingBufferCreateInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                stagingBufferCreateInfo.size
            );
            memcpy(
                writeLocation,
                vertices.data(),
                stagingBufferCreateInfo.size
            );
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            Vulkan::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.physicalDevice   = m_internals.physicalDevice;
            bufferCreateInfo.logicalDevice    = m_internals.logicalDevice;;
            bufferCreateInfo.size             = stagingBufferCreateInfo.size;
            bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                                vk::BufferUsageFlagBits::eVertexBuffer;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Vulkan::Buffer::init(m_vertexBuffer, bufferCreateInfo);

            Vulkan::Buffer::copy(
                stagingBuffer,
                m_vertexBuffer,
                bufferCreateInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Vulkan::Buffer::destroy(
                m_internals.logicalDevice,
                stagingBuffer
            );
        }
    }
}