#include "Chicane/Runtime/Renderer/Vulkan/Layer/Sky.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor/Sky.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LSky::LSky()
            : Super("Engine_Sky"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_graphicsPipeline(nullptr),
            m_frameDescriptor({}),
            m_textureDescriptor({}),
            m_sky(nullptr),
            m_asset(nullptr),
            m_modelVertexBuffer({}),
            m_modelIndexBuffer({}),
            m_clearValues({}),
            m_modelManager(Box::getModelManager()),
            m_textureManager(Box::getTextureManager())
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

            loadEvents();
        }

        LSky::~LSky()
        {
            m_internals.logicalDevice.waitIdle();

            destroyTextureResources();
            destroyModelData();

            m_graphicsPipeline.reset();
        }

        bool LSky::onInit()
        {
            if (!m_asset)
            {
                return false;
            }

            initFrameResources();
            initTextureResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildTextureData();
            buildModelData();

            return true;
        }

        bool LSky::onDestroy()
        {
            destroyFrameResources();

            return true;
        }

        bool LSky::onRebuild()
        {
            if (!m_asset)
            {
                return false;
            }

            initFramebuffers();
            initFrameResources();

            return true;
        }

        void LSky::onRender(void* outData)
        {
            if (!is(RendererLayerStatus::Running))
            {
                return;
            }

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

                // Frame
                m_graphicsPipeline->bindDescriptorSet(commandBuffer, 0, frame.getDescriptorSet(m_id));

                // Textures
                m_sky->bind(commandBuffer, m_graphicsPipeline->layout);

                //Model
                vk::Buffer vertexBuffers[] = { m_modelVertexBuffer.instance };
                vk::DeviceSize offsets[]   = { 0 };

                commandBuffer.bindVertexBuffers(
                    0,
                    1,
                    vertexBuffers,
                    offsets
                );

                commandBuffer.bindIndexBuffer(
                    m_modelIndexBuffer.instance,
                    0,
                    vk::IndexType::eUint32
                );

                commandBuffer.drawIndexed(
                    static_cast<std::uint32_t>(m_modelManager->getInstance(m_asset->getModel()).indices.size()),
                    1,
                    0,
                    0,
                    0
                );
            commandBuffer.endRenderPass();
        }

        void LSky::loadEvents()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            Application::watchScene(
                [this](Scene* inLevel) {
                    if (!inLevel)
                    {
                        return;
                    }

                    inLevel->watchActors(
                        [this](const std::vector<Actor*>& inActors)
                        {
                            const std::vector<ASky*> skies = Application::getScene()->getActors<ASky>();

                            if (skies.empty())
                            {
                                destroy();

                                return;
                            }

                            skies.front()->watchSky(
                                [this](const Chicane::Box::Sky* inSky)
                                {
                                    if (!is(RendererLayerStatus::Offline) || !inSky)
                                    {
                                        return;
                                    }

                                    m_asset = inSky;

                                    init();
                                }
                            );
                        }
                    );
                }
            );
        }

        void LSky::initFrameResources()
        {
            if (!is(RendererLayerStatus::Offline) && !is(RendererLayerStatus::Initialized))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 1;

            /// Camera
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                frameLayoutBidings
            );

            Descriptor::PoolCreateInfo frameDescriptorPoolCreateInfo;
            frameDescriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
            frameDescriptorPoolCreateInfo.sizes.push_back(
                { vk::DescriptorType::eUniformBuffer, frameDescriptorPoolCreateInfo.maxSets }
            );

            Descriptor::initPool(
                m_frameDescriptor.pool,
                m_internals.logicalDevice,
                frameDescriptorPoolCreateInfo
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

                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = descriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);
            }
        }

        void LSky::destroyFrameResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );
            m_internals.logicalDevice.destroyDescriptorPool(
                m_frameDescriptor.pool
            );
        }

        void LSky::initTextureResources()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
            materialLayoutBidings.count = 1;
            materialLayoutBidings.indices.push_back(0);
            materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            materialLayoutBidings.counts.push_back(1);
            materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Descriptor::initSetLayout(
                m_textureDescriptor.setLayout,
                m_internals.logicalDevice,
                materialLayoutBidings
            );

            Descriptor::PoolCreateInfo materialDescriptorPoolCreateInfo;
            materialDescriptorPoolCreateInfo.maxSets = 1;
            materialDescriptorPoolCreateInfo.sizes.push_back(
                { vk::DescriptorType::eCombinedImageSampler, 1 }
            );

            Descriptor::initPool(
                m_textureDescriptor.pool,
                m_internals.logicalDevice,
                materialDescriptorPoolCreateInfo
            );
        }

        void LSky::destroyTextureResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_textureDescriptor.setLayout
            );
            m_internals.logicalDevice.destroyDescriptorPool(
                m_textureDescriptor.pool
            );
        }

        void LSky::initGraphicsPipeline()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/sky.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/sky.frag.spv";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Shader::StageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);
            shaders.push_back(fragmentShader);

            std::vector<vk::DescriptorSetLayout> descriptorSetLayouts = {};
            descriptorSetLayouts.push_back(m_frameDescriptor.setLayout);
            descriptorSetLayouts.push_back(m_textureDescriptor.setLayout);

            GraphicsPipeline::Attachment colorAttachment = {};
            colorAttachment.type          = GraphicsPipeline::Attachment::Type::Color;
            colorAttachment.format        = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            std::vector<GraphicsPipeline::Attachment> attachments = {};
            attachments.push_back(colorAttachment);

            GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.bHasVertices             = true;
            createInfo.bHasDepthWrite           = false;
            createInfo.bHasBlending             = false;
            createInfo.logicalDevice            = m_internals.logicalDevice;
            createInfo.shaders                  = shaders;
            createInfo.extent                   = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts     = descriptorSetLayouts;
            createInfo.attachments              = attachments;
            createInfo.rasterizaterizationState = GraphicsPipeline::createRasterizationState(vk::PolygonMode::eFill);

            m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
        }

        void LSky::initFramebuffers()
        {
            if (!is(RendererLayerStatus::Offline) && !is(RendererLayerStatus::Initialized))
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

        void LSky::buildTextureData()
        {
            if (!m_asset)
            {
                return;
            }

            Sky::CreateInfo createInfo = {};
            createInfo.images              = {};
            createInfo.logicalDevice       = m_internals.logicalDevice;
            createInfo.physicalDevice      = m_internals.physicalDevice;
            createInfo.commandBuffer       = m_internals.mainCommandBuffer;
            createInfo.queue               = m_internals.graphicsQueue;
            createInfo.descriptorPool      = m_textureDescriptor.pool;
            createInfo.descriptorSetLayout = m_textureDescriptor.setLayout;

            Box::TextureManager* textureManager = m_textureManager;

            for (const auto& [side, texture] : m_asset->getSides())
            {
                textureManager->activate(texture);

                createInfo.images.insert(
                    std::make_pair(
                        side,
                        textureManager->getData(texture)
                    )
                );
            }

            m_sky.reset();
            m_sky = std::make_unique<Sky::Instance>(createInfo);
        }

        void LSky::buildModelVertexBuffer()
        {
            if (!m_asset)
            {
                return;
            }

            const auto& vertices = m_modelManager->getInstance(m_asset->getModel()).vertices;

            BufferCreateInfo createInfo;
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Chicane::Vertex) * vertices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                createInfo.size
            );
            memcpy(writeLocation, vertices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_modelVertexBuffer.init(createInfo);

            stagingBuffer.copy(
                m_modelVertexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );
            stagingBuffer.destroy(m_internals.logicalDevice);
        }

        void LSky::buildModelIndexBuffer()
        {
            if (!m_asset)
            {
                return;
            }

            const auto& indices = m_modelManager->getInstance(m_asset->getModel()).indices;

            BufferCreateInfo createInfo;
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(std::uint32_t) * indices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                createInfo.size
            );
            memcpy(writeLocation, indices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_modelIndexBuffer.init(createInfo);

            stagingBuffer.copy(
                m_modelIndexBuffer,
                createInfo.size,
                m_internals.graphicsQueue,
                m_internals.mainCommandBuffer
            );
            stagingBuffer.destroy(m_internals.logicalDevice);
        }

        void LSky::buildModelData()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LSky::destroyModelData()
        {
            m_internals.logicalDevice.waitIdle();

            m_modelVertexBuffer.destroy(m_internals.logicalDevice);
            m_modelIndexBuffer.destroy(m_internals.logicalDevice);
        }

        void LSky::rebuildModelData()
        {
            destroyModelData();
            buildModelData();
        }
    }
}