#include "Chicane/Runtime/Renderer/Vulkan/Layer/Grid.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LGrid::LGrid()
            : Layer::Instance("UI"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_clearValues({}),
            m_view(nullptr),
            m_components({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));

            loadEvents();
        }

        LGrid::~LGrid()
        {
            if (is(Layer::Status::Offline))
            {
                return;
            }

            m_internals.logicalDevice.waitIdle();

            destroyData();

            m_graphicsPipeline.reset();
        }

        void LGrid::build()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            initGraphicsPipeline();
            initFramebuffers();

            buildData();

            setStatus(Layer::Status::Running);
        }

        void LGrid::destroy()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            setStatus(Layer::Status::Idle);
        }

        void LGrid::rebuild()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            initFramebuffers();

            setStatus(Layer::Status::Running);
        }

        void LGrid::render(void* outData)
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

                // UI
                renderComponents(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void LGrid::loadEvents()
        {
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            setStatus(Layer::Status::Idle);

            Grid::watchActiveView(
                [&](Grid::View* inView)
                {
                    m_view = inView;

                    m_components.clear();

                    if (!m_view)
                    {
                        return;
                    }

                    m_view->watchSize(
                        [&](const Vec<2, float> inSize)
                        {
                            m_components = m_view->getDrawableChildren();

                            if (m_components.empty())
                            {
                                destroy();

                                return;
                            }

                            if (is(Layer::Status::Running))
                            {
                                destroy();
                                rebuild();

                                return;
                            }

                            setStatus(Layer::Status::Initialized);
                            build();
                        }
                    );
                }
            );
        }

        void LGrid::initGraphicsPipeline()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            // Shader
            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Engine/Shaders/Vulkan/grid.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path = "Engine/Shaders/Vulkan/grid.frag.spv";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Shader::StageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);
            shaders.push_back(fragmentShader);

            // Push Constants
            vk::PushConstantRange pushConstantRange = {};
            pushConstantRange.offset     = 0;
            pushConstantRange.size       = sizeof(PushConstant);
            pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;

            std::vector<vk::PushConstantRange> pushConstantRanges = {};
            pushConstantRanges.push_back(pushConstantRange);

            // Attachments
            GraphicsPipeline::Attachment colorAttachment = {};
            colorAttachment.type          = GraphicsPipeline::Attachment::Type::Color;
            colorAttachment.format        = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            std::vector<GraphicsPipeline::Attachment> attachments = {};
            attachments.push_back(colorAttachment);

            GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.bHasVertices             = true;
            createInfo.bHasDepthWrite           = false;
            createInfo.bHasBlending             = true;
            createInfo.logicalDevice            = m_internals.logicalDevice;
            createInfo.shaders                  = shaders;
            createInfo.extent                   = m_internals.swapchain->extent;
            createInfo.pushConstantRanges       = pushConstantRanges;
            createInfo.attachments              = attachments;
            createInfo.rasterizaterizationState = GraphicsPipeline::createRasterizationState(vk::PolygonMode::eFill);

            m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
        }

        void LGrid::initFramebuffers()
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
                createInfo.attachments.push_back(frame.colorImage.view);

                Frame::Buffer::init(frame, createInfo);
            }
        }

        void LGrid::buildVertexBuffer()
        {
            std::vector<Chicane::Vertex> vertices = {};

            for (const Grid::Component* component : m_components)
            {
                const std::vector<Chicane::Vertex> primitives = component->getPrimitive();

                if (primitives.empty())
                {
                    continue;
                }

                vertices.insert(
                    vertices.end(),
                    primitives.begin(),
                    primitives.end()
                );
            }

            Buffer::CreateInfo createInfo = {};
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Chicane::Vertex) * vertices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                createInfo.size
            );
            memcpy(writeLocation, vertices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Buffer::init(m_vertexBuffer, createInfo);
            Buffer::copy(
                stagingBuffer,
                m_vertexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Buffer::destroy(m_internals.logicalDevice, stagingBuffer);
        }

        void LGrid::buildData()
        {
            buildVertexBuffer();
        }

        void LGrid::destroyData()
        {
            m_internals.logicalDevice.waitIdle();

            Buffer::destroy(m_internals.logicalDevice, m_vertexBuffer);
        }

        void LGrid::rebuildData()
        {
            destroyData();
            buildData();
        }

        void LGrid::renderComponents(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Buffer vertexBuffers[] = { m_vertexBuffer.instance };
            vk::DeviceSize offsets[]   = { 0 };

            inCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            std::uint32_t firstVertex = 0;
            std::uint32_t firstIndex  = 0;

            for (const Grid::Component* component : m_components)
            {
                if (component->isPrimitiveEmpty())
                {
                    continue;
                }

                PushConstant pushConstant = {};
                pushConstant.size     = component->getSize();
                pushConstant.position = component->getPosition();

                inCommandBuffer.pushConstants(
                    m_graphicsPipeline->layout,
                    vk::ShaderStageFlagBits::eVertex,
                    0,
                    sizeof(PushConstant),
                    &pushConstant
                );
                inCommandBuffer.draw(component->getPrimitiveCount(), 1, firstVertex, firstIndex);

                firstVertex += component->getPrimitiveCount();
                firstIndex  += 1;
            }
        }
    }
}