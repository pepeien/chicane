#include "Chicane/Renderer/Vulkan/Layer/Sky.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LSky::LSky()
            : Layer::Instance("Sky"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

            loadEvents();
        }

        LSky::~LSky()
        {
            if (is(Layer::Status::Offline))
            {
                return;
            }

            m_internals.logicalDevice.waitIdle();

            destroyTextureResources();
            destroyModelData();

            m_graphicsPipeline.reset();
        }

        void LSky::build()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            initFrameResources();
            initTextureResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildTextureData();
            buildModelData();

            setStatus(Layer::Status::Running);
        }

        void LSky::destroy()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            destroyFrameResources();

            setStatus(Layer::Status::Idle);
        }

        void LSky::rebuild()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            initFramebuffers();
            initFrameResources();

            setStatus(Layer::Status::Running);
        }

        void LSky::render(void* outData)
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
                    Box::getModelManager()->getInstance(m_asset->getModel()).indices.size(),
                    1,
                    0,
                    0,
                    0
                );
            commandBuffer.endRenderPass();
        }

        void LSky::loadEvents()
        {
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            Application::watchLevel(
                [this](Level* inLevel) {
                    if (!inLevel)
                    {
                        return;
                    }

                    inLevel->watchActors(
                        [this](const std::vector<Actor*>& inActors)
                        {
                            std::vector<ASky*> skies = Application::getLevel()->getActors<ASky>();

                            if (skies.empty())
                            {
                                destroy();

                                return;
                            }

                            if (!is(Layer::Status::Idle))
                            {
                                return;
                            }

                            skies.front()->watchSky(
                                [this](const Chicane::Box::Sky::Instance* inSky)
                                {
                                    if (!is(Layer::Status::Idle) || !inSky)
                                    {
                                        return;
                                    }

                                    setStatus(Layer::Status::Initialized);

                                    m_asset = inSky;

                                    build();
                                }
                            );
                        }
                    );
                }
            );

            setStatus(Layer::Status::Idle);
        }

        void LSky::initFrameResources()
        {
            if (is(Layer::Status::Running))
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
                { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = frameDescriptorPoolCreateInfo.maxSets }
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

                vk::WriteDescriptorSet writeDescriptorSet;
                writeDescriptorSet.dstSet          = descriptorSet;
                writeDescriptorSet.dstBinding      = 0;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.descriptorType  = vk::DescriptorType::eUniformBuffer;
                writeDescriptorSet.pBufferInfo     = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(writeDescriptorSet);
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
            if (!is(Layer::Status::Initialized))
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
                { .type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1 }
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
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Content/Engine/Vulkan/Shaders/sky.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path = "Content/Engine/Vulkan/Shaders/sky.frag.spv";
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

        void LSky::buildTextureData()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            if (m_sky.get() != nullptr)
            {
                m_sky.reset();
            }

            Sky::CreateInfo createInfo = {};
            createInfo.images              = {};
            createInfo.logicalDevice       = m_internals.logicalDevice;
            createInfo.physicalDevice      = m_internals.physicalDevice;
            createInfo.commandBuffer       = m_internals.mainCommandBuffer;
            createInfo.queue               = m_internals.graphicsQueue;
            createInfo.descriptorPool      = m_textureDescriptor.pool;
            createInfo.descriptorSetLayout = m_textureDescriptor.setLayout;

            Box::Texture::Manager* textureManager = Box::getTextureManager();

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

            m_sky = std::make_unique<Sky::Instance>(createInfo);
        }

        void LSky::buildModelVertexBuffer()
        {
            const auto& vertices = Box::getModelManager()->getInstance(m_asset->getModel()).vertices;

            Buffer::CreateInfo createInfo;
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

        void LSky::buildModelIndexBuffer()
        {
            const auto& indices = Box::getModelManager()->getInstance(m_asset->getModel()).indices;

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

        void LSky::buildModelData()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LSky::destroyModelData()
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

        void LSky::rebuildModelData()
        {
            destroyModelData();
            buildModelData();
        }
    }
}