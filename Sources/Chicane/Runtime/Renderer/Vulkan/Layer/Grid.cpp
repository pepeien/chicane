#include "Chicane/Runtime/Renderer/Vulkan/Layer/Grid.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LGrid::LGrid()
            : Layer::Instance("UI"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_graphicsPipeline(nullptr),
            m_clearValues({}),
            m_view(nullptr),
            m_draws({})
        {
            m_clearValues.emplace_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));

            loadEvents();
        }

        LGrid::~LGrid()
        {
            m_internals.logicalDevice.waitIdle();

            destroyDraws();

            m_graphicsPipeline.reset();
        }

        bool LGrid::onInit()
        {
            initGraphicsPipeline();
            initFramebuffers();

            return true;
        }

        bool LGrid::onRebuild()
        {
            initFramebuffers();

            return true;
        }

        bool LGrid::onSetup()
        {
            destroyDraws();

            if (m_view == nullptr)
            {
                return false;
            }

            buildDraws(m_view->getChildren());

            if (m_draws.empty())
            {
                return false;
            }
    
            return true;
        }

        void LGrid::onRender(void* outData)
        {
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

                // UI
                renderComponents(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void LGrid::loadEvents()
        {
            Application::watchView(
                [&](Grid::View* inView)
                {
                    m_view = inView;
                }
            );
        }

        void LGrid::initGraphicsPipeline()
        {
            // Shader
            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Contents/Shaders/Vulkan/grid.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path = "Contents/Shaders/Vulkan/grid.frag.spv";
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

        void LGrid::renderComponents(const vk::CommandBuffer& inCommandBuffer)
        {
            const Vec2& rootSize = m_view->getSize();

            for (const Draw& draw : m_draws)
            {
                const Vec2& size     = draw.size;
                const Vec2& position = draw.position;

                PushConstant pushConstant = {};
                pushConstant.size       = size / rootSize;
                pushConstant.position.x = ((2.0f * position.x) / rootSize.x) - 1.0f;
                pushConstant.position.y = ((2.0f * position.y) / rootSize.y) - 1.0f;

                vk::DeviceSize offsets[] = { 0 };

                inCommandBuffer.bindVertexBuffers(0, 1, &draw.vertexBuffer.instance, offsets);

                inCommandBuffer.bindIndexBuffer(draw.indexBuffer.instance, 0, vk::IndexType::eUint32);

                inCommandBuffer.pushConstants(
                    m_graphicsPipeline->layout,
                    vk::ShaderStageFlagBits::eVertex,
                    0,
                    sizeof(PushConstant),
                    &pushConstant
                );

                inCommandBuffer.drawIndexed(draw.indexCount, 1, 0, 0, 0);
            }
        }

        void LGrid::setDrawVertexBuffer(Draw& outDraw, const std::vector<Chicane::Vertex>& inVertices)
        {
            if (inVertices.empty())
            {
                return;
            }

            Buffer::CreateInfo createInfo = {};
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Chicane::Vertex) * inVertices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, createInfo);

            memcpy(
                m_internals.logicalDevice.mapMemory(
                    stagingBuffer.memory,
                    0,
                    createInfo.size
                ),
                inVertices.data(),
                createInfo.size
            );
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Buffer::init(outDraw.vertexBuffer, createInfo);
            Buffer::copy(
                stagingBuffer,
                outDraw.vertexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Buffer::destroy(m_internals.logicalDevice, stagingBuffer);
        }

        void LGrid::setDrawIndexBuffer(Draw& outDraw, const std::vector<std::uint32_t>& inIndices)
        {
            if (inIndices.empty())
            {
                return;
            }

            Buffer::CreateInfo createInfo;
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(std::uint32_t) * inIndices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, createInfo);

            memcpy(
                m_internals.logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size),
                inIndices.data(),
                createInfo.size
            );
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            Buffer::init(outDraw.indexBuffer, createInfo);
            Buffer::copy(
                stagingBuffer,
                outDraw.indexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );

            Buffer::destroy(m_internals.logicalDevice, stagingBuffer);
        }

        void LGrid::buildDraws(const std::vector<Grid::Component*>& inComponents)
        {
            if (inComponents.empty())
            {
                return;
            }

            for (Grid::Component* component : inComponents)
            {
                component->refresh();

                if (!component->isVisible())
                {
                    continue;
                }

                if (component->isDrawable())
                {
                    const Grid::Primitive primitive = component->getPrimitive();

                    Draw drawData = {};
                    drawData.size        = component->getSize();
                    drawData.position    = component->getPosition();
                    drawData.vertexCount = primitive.vertices.size();
                    drawData.indexCount  = primitive.indices.size();
                    setDrawVertexBuffer(drawData, primitive.vertices);
                    setDrawIndexBuffer(drawData, primitive.indices);

                    m_draws.push_back(drawData);
                }

                buildDraws(component->getChildren());
            }
        }

        void LGrid::destroyDraws()
        {
            m_internals.logicalDevice.waitIdle();

            for (const Draw& draw : m_draws)
            {
                Buffer::destroy(m_internals.logicalDevice, draw.vertexBuffer);
                Buffer::destroy(m_internals.logicalDevice, draw.indexBuffer);
            }

            m_draws.clear();
        }
    }
}