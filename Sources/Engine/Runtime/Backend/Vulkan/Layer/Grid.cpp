#include "Chicane/Runtime/Backend/Vulkan/Layer/Grid.hpp"

#include "Chicane/Grid/Component/Character.hpp"
#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LGrid::LGrid()
            : Super("Engine_UI"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_graphicsPipeline(nullptr),
            m_clearValues({}),
            m_view(nullptr),
            m_draws({})
        {
            m_clearValues.emplace_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));

            loadEvents();

            init();
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
            if (!m_view)
            {
                return false;
            }

            refreshDraw(m_view);

            if (m_draws.empty())
            {
                return false;
            }
    
            return true;
        }

        void LGrid::onRender(void* outData)
        {
            RendererData* data = (RendererData*) outData;
            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Frame::Instance& frame = data->frame;

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
                    destroyDraws();

                    m_view = inView;

                    if (!m_view)
                    {
                        return;
                    }

                    m_view->watchChildren(
                        [this](Grid::Component* inChild)
                        {
                            if (!hasDraw(inChild))
                            {
                                return;
                            }

                            getDraw(inChild).bIsOutdated = true;
                        }
                    );
                }
            );
        }

        void LGrid::initGraphicsPipeline()
        {
            // Shader
            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Grid.vert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Grid.frag";
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
                if (!draw.bIsDrawable)
                {
                    continue;
                }

                const Vec2& size     = draw.size;
                const Vec2& position = draw.position;

                PushConstant pushConstant = {};
                pushConstant.size       = size / rootSize;
                pushConstant.position.x = ((2.0f * position.x) / rootSize.x) - 1.0f;
                pushConstant.position.y = ((2.0f * position.y) / rootSize.y) - 1.0f;

                vk::DeviceSize offset = 0;

                inCommandBuffer.bindVertexBuffers(0, 1, &draw.vertexBuffer.instance, &offset);

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
            destroyDrawVertexBuffer(outDraw);

            if (inVertices.empty())
            {
                return;
            }

            BufferCreateInfo createInfo = {};
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Chicane::Vertex) * inVertices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer stagingBuffer;
            stagingBuffer.init(createInfo);

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

            outDraw.vertexBuffer.init(createInfo);

            stagingBuffer.copy(
                outDraw.vertexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );
            stagingBuffer.destroy(m_internals.logicalDevice);
        }

        void LGrid::destroyDrawVertexBuffer(Draw& outDraw)
        {
            m_internals.logicalDevice.waitIdle();

            outDraw.vertexBuffer.destroy(m_internals.logicalDevice);
        }

        void LGrid::setDrawIndexBuffer(Draw& outDraw, const std::vector<std::uint32_t>& inIndices)
        {
            destroyDrawIndexBuffer(outDraw);

            if (inIndices.empty())
            {
                return;
            }

            BufferCreateInfo createInfo;
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(std::uint32_t) * inIndices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer stagingBuffer;
            stagingBuffer.init(createInfo);

            memcpy(
                m_internals.logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size),
                inIndices.data(),
                createInfo.size
            );
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            outDraw.indexBuffer.init(createInfo);

            stagingBuffer.copy(
                outDraw.indexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );
            stagingBuffer.destroy(m_internals.logicalDevice);
        }

        void LGrid::destroyDrawIndexBuffer(Draw& outDraw)
        {
            m_internals.logicalDevice.waitIdle();

            outDraw.indexBuffer.destroy(m_internals.logicalDevice);
        }

        bool LGrid::hasDraw(Grid::Component* inComponent)
        {
            if (!inComponent)
            {
                return false;
            }

            return std::find_if(
                m_draws.begin(),
                m_draws.end(),
                [inComponent](const Draw& draw) { return draw.component == inComponent; }
            ) != m_draws.end();
        }

        LGrid::Draw& LGrid::getDraw(Grid::Component* inComponent)
        {
            return *std::find_if(
                m_draws.begin(),
                m_draws.end(),
                [inComponent](const Draw& draw) { return draw.component == inComponent; }
            );
        }

        void LGrid::buildDraw(Grid::Component* inComponent)
        {
            if (!inComponent)
            {
                return;
            }

            if (!inComponent->isRoot())
            {
                if (!hasDraw(inComponent))
                {
                    Draw draw = {};
                    draw.component = inComponent;

                    m_draws.push_back(draw);
                }
            }
    
            for (Grid::Component* component : inComponent->getChildren())
            {
                buildDraw(component);
            }
        }

        void LGrid::refreshDraw(Grid::Component* inComponent)
        {
            if (!inComponent)
            {
                return;
            }

            if (!inComponent->isRoot())
            {
                if (!hasDraw(inComponent))
                {
                    buildDraw(inComponent);
                }

                Draw& draw = getDraw(inComponent);

                if (draw.bIsOutdated)
                {
                    if (inComponent->isDrawable())
                    {
                        Grid::Primitive primitive = inComponent->getPrimitive();

                        if (typeid(*inComponent) == typeid(Grid::Character))
                        {
                            for (Vertex& vertex : primitive.vertices)
                            {
                                vertex.position.y = -vertex.position.y;
                            }
                        }

                        draw.size        = inComponent->getSize();
                        draw.position    = inComponent->getPosition();
                        draw.vertexCount = primitive.vertices.size();
                        draw.indexCount  = primitive.indices.size();
                        draw.bIsDrawable = true;

                        setDrawVertexBuffer(draw, primitive.vertices);
                        setDrawIndexBuffer(draw, primitive.indices);
                    }
                    else
                    {
                        draw.size        = Vec2::Zero;
                        draw.position    = Vec2::Zero;
                        draw.vertexCount = 0;
                        draw.indexCount  = 0;
                        draw.bIsDrawable = false;
                        destroyDrawVertexBuffer(draw);
                        destroyDrawIndexBuffer(draw);
                    }

                    draw.bIsOutdated = false;
                }
            }

            for (Grid::Component* component : inComponent->getChildren())
            {
                refreshDraw(component);
            }
        }

        void LGrid::destroyDraws()
        {
            for (Draw& draw : m_draws)
            {
                destroyDrawVertexBuffer(draw);
                destroyDrawIndexBuffer(draw);
            }

            m_draws.clear();
        }
    }
}