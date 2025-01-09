#include "Chicane/Renderer/Vulkan/Layer/LevelLayer.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game/Transformable/Component/MeshComponent.hpp"
#include "Chicane/Renderer/Vertex.hpp"
#include "Chicane/Renderer/Vulkan/Texture.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LevelLayer::LevelLayer()
            : Layer::Instance("Level"),
            m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
            m_clearValues({}),
            m_textureManager(Loader::getTextureManager()),
            m_modelManager(Loader::getModelManager())
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
            m_clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

            loadEvents();
        }

        LevelLayer::~LevelLayer()
        {
            m_internals.logicalDevice.waitIdle();

            destroyTextureResources();
            destroyModelData();

            m_graphicsPipeline.reset();
        }

        void LevelLayer::build()
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

            setupFrames();

            setStatus(Layer::Status::Running);
        }

        void LevelLayer::destroy()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            destroyFrameResources();

            setStatus(Layer::Status::Idle);
        }

        void LevelLayer::rebuild()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            initFrameResources();
            initFramebuffers();
            setupFrames();

            setStatus(Layer::Status::Running);
        }

        void LevelLayer::setup(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Vulkan::Frame::Instance* outFrame = (Vulkan::Frame::Instance*) outData;
            outFrame->updateMeshData(m_meshes);
        }

        void LevelLayer::render(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Vulkan::Renderer::Data* data = (Vulkan::Renderer::Data*) outData;

            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Frame::Instance& frame = data->frame;

            vk::RenderPassBeginInfo renderPassBeginInfo {};
            renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
            renderPassBeginInfo.framebuffer         = frame.getFramebuffer(m_id);
            renderPassBeginInfo.renderArea.offset.x = 0;
            renderPassBeginInfo.renderArea.offset.y = 0;
            renderPassBeginInfo.renderArea.extent   = data->swapChainExtent;
            renderPassBeginInfo.clearValueCount     = static_cast<std::uint32_t>(m_clearValues.size());
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

                renderTextures(commandBuffer);
                renderModels(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void LevelLayer::loadEvents()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            Application::watchLevel(
                [this](Level* inLevel) {
                    m_level = inLevel;

                    if (!inLevel)
                    {
                        return;
                    }

                    m_level->watchComponents(
                        [this](const std::vector<Component*>& inComponents) {
                            updateMeshes(inComponents);
                        }
                    );
                }
            );

            m_textureManager->watchChanges(
                [&](Manager::EventType inEvent)
                {
                    if (is(Layer::Status::Idle) && !m_modelManager->isEmpty())
                    {
                        setStatus(Layer::Status::Initialized);

                        build();

                        return;
                    }

                    if (is(Layer::Status::Running))
                    {
                        buildTextureData();
                    }
                }
            );

            m_modelManager->watchChanges(
                [&](Manager::EventType inEvent)
                {
                    if (inEvent != Manager::EventType::Activation)
                    {
                        return;
                    }

                    if (is(Layer::Status::Idle) && !m_textureManager->isEmpty())
                    {
                        setStatus(Layer::Status::Initialized);

                        build();

                        return;
                    }

                    if (is(Layer::Status::Running))
                    {
                        rebuildModelData();

                        return;
                    }
                }
            );
        }

        void LevelLayer::initFrameResources()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 2;

            /// Camera
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            /// Model
            frameLayoutBidings.indices.push_back(1);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            Vulkan::Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                frameLayoutBidings
            );

            Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.size  = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
            descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);
            descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eStorageBuffer);

            Vulkan::Descriptor::initPool(
                m_frameDescriptor.pool,
                m_internals.logicalDevice,
                descriptorPoolCreateInfo
            );

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                vk::DescriptorSet levelDescriptorSet;
                Vulkan::Descriptor::allocalteSet(
                    levelDescriptorSet,
                    m_internals.logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                frame.addDescriptorSet(m_id, levelDescriptorSet);
        
                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = levelDescriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);

                vk::WriteDescriptorSet modelWriteInfo;
                modelWriteInfo.dstSet          = levelDescriptorSet;
                modelWriteInfo.dstBinding      = 1;
                modelWriteInfo.dstArrayElement = 0;
                modelWriteInfo.descriptorCount = 1;
                modelWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                modelWriteInfo.pBufferInfo     = &frame.modelDescriptorBufferInfo;
                frame.addWriteDescriptorSet(modelWriteInfo);
            }
        }

        void LevelLayer::destroyFrameResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );
            m_internals.logicalDevice.destroyDescriptorPool(
                m_frameDescriptor.pool
            );
        }

        void LevelLayer::initTextureResources()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Vulkan::Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
            materialLayoutBidings.count = 1;

            materialLayoutBidings.indices.push_back(0);
            materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            materialLayoutBidings.counts.push_back(m_textureManager->getActiveCount());
            materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Vulkan::Descriptor::initSetLayout(
                m_textureDescriptor.setLayout,
                m_internals.logicalDevice,
                materialLayoutBidings
            );

            Vulkan::Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.size = TEXTURE_MAX_COUNT;
            descriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

            Vulkan::Descriptor::initPool(
                m_textureDescriptor.pool,
                m_internals.logicalDevice,
                descriptorPoolCreateInfo
            );
        }

        void LevelLayer::destroyTextureResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_textureDescriptor.setLayout
            );

            m_internals.logicalDevice.destroyDescriptorPool(
                m_textureDescriptor.pool
            );
        }

        void LevelLayer::initGraphicsPipeline()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Vulkan::GraphicsPipeline::Attachment colorAttachment {};
            colorAttachment.format        = m_internals.swapchain->format;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;

            Vulkan::GraphicsPipeline::Attachment depthAttachment {};
            depthAttachment.format        = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;

            Vulkan::GraphicsPipeline::CreateInfo createInfo {};
            createInfo.bHasVertices          = true;
            createInfo.bHasDepth             = true;
            createInfo.logicalDevice         = m_internals.logicalDevice;
            createInfo.vertexShaderPath      = "Content/Engine/Vulkan/Shaders/level.vert.spv";
            createInfo.fragmentShaderPath    = "Content/Engine/Vulkan/Shaders/level.frag.spv";
            createInfo.bindingDescription    = Vulkan::Vertex::getBindingDescription();
            createInfo.attributeDescriptions = Vulkan::Vertex::getAttributeDescriptions();
            createInfo.extent                = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts  = { m_frameDescriptor.setLayout, m_textureDescriptor.setLayout };
            createInfo.colorAttachment       = colorAttachment;
            createInfo.depthAttachment       = depthAttachment;
            createInfo.polygonMode           = vk::PolygonMode::eFill;

            m_graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline::Instance>(createInfo);
        }

        void LevelLayer::initFramebuffers()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                Vulkan::Frame::Buffer::CreateInfo framebufferCreateInfo {};
                framebufferCreateInfo.id              = m_id;
                framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
                framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
                framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;
                framebufferCreateInfo.attachments.push_back(frame.imageView);
                framebufferCreateInfo.attachments.push_back(frame.depth.imageView);

                Vulkan::Frame::Buffer::init(frame, framebufferCreateInfo);
            }
        }

        void LevelLayer::buildTextureData()
        {
            Texture::CreateInfo createInfo {};
            createInfo.logicalDevice  = m_internals.logicalDevice;
            createInfo.physicalDevice = m_internals.physicalDevice;
            createInfo.commandBuffer  = m_internals.mainCommandBuffer;
            createInfo.queue          = m_internals.graphicsQueue;

            std::vector<vk::DescriptorImageInfo> imageInfos {};

            for (const std::string& id : m_textureManager->getActiveIds())
            {
                createInfo.image = m_textureManager->getData(id);

                m_textures.insert(
                    std::make_pair(
                        id,
                        std::make_unique<Texture::Instance>(createInfo)
                    )
                );

                Image::Data image = m_textures.at(id)->getImage();

                vk::DescriptorImageInfo info {};
                info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                info.imageView   = image.view;
                info.sampler     = image.sampler;

                imageInfos.push_back(info);
            }

            Descriptor::allocalteSet(
                m_textureDescriptor.set,
                createInfo.logicalDevice,
                m_textureDescriptor.setLayout,
                m_textureDescriptor.pool
            );

            vk::WriteDescriptorSet descriptorSet {};
            descriptorSet.dstSet          = m_textureDescriptor.set;
            descriptorSet.dstBinding      = 0;
            descriptorSet.dstArrayElement = 0;
            descriptorSet.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            descriptorSet.descriptorCount = imageInfos.size();
            descriptorSet.pImageInfo      = imageInfos.data();

            createInfo.logicalDevice.updateDescriptorSets(
                descriptorSet,
                nullptr
            );
        }

        void LevelLayer::renderTextures(const vk::CommandBuffer& inCommandBuffer)
        {
            for (const std::string& id : m_textureManager->getActiveIds())
            {
                inCommandBuffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    m_graphicsPipeline->layout,
                    1,
                    m_textureDescriptor.set,
                    nullptr
                );
            }
        }

        void LevelLayer::buildModelVertexBuffer()
        {
            const auto& vertices = m_modelManager->getVertices();

            Buffer::CreateInfo createInfo;
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Chicane::Vertex::Instance) * vertices.size();
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                createInfo.size
            );
            memcpy(
                writeLocation,
                vertices.data(),
                createInfo.size
            );
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

        void LevelLayer::buildModelIndexBuffer()
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

        void LevelLayer::buildModelData()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LevelLayer::destroyModelData()
        {
            m_internals.logicalDevice.waitIdle();

            Vulkan::Buffer::destroy(
                m_internals.logicalDevice,
                m_modelVertexBuffer
            );
            Vulkan::Buffer::destroy(
                m_internals.logicalDevice,
                m_modelIndexBuffer
            );
        }

        void LevelLayer::rebuildModelData()
        {
            destroyModelData();

            if (m_modelManager->isEmpty())
            {
                return;
            }

            buildModelData();
        }

        void LevelLayer::renderModels(const vk::CommandBuffer& inCommandBuffer)
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
                const Model::Data& data = m_modelManager->getData(id);

                inCommandBuffer.drawIndexed(
                    data.indexCount,
                    m_modelManager->getUseCount(id),
                    data.firstIndex,
                    0,
                    m_modelManager->getFirstInstance(id)
                );
            }
        }

        void LevelLayer::setupFrames()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                frame.setupMeshData(m_meshes);
            }
        }

        void LevelLayer::setFramesAsDirty()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                frame.setAsDirty();
            }
        }

        void LevelLayer::updateMeshes(const std::vector<Component*>& inComponents)
        {
            std::vector<MeshComponent*> nextMeshes {};
            nextMeshes.reserve(inComponents.size());

            for (Component* component : inComponents)
            {
                if (typeid(*component) != typeid(MeshComponent))
                {
                    continue;
                }

                MeshComponent* mesh = static_cast<MeshComponent*>(component);

                if (!mesh->isDrawable())
                {
                    continue;
                }

                nextMeshes.emplace_back(mesh);
            }

            if (m_meshes.size() == nextMeshes.size())
            {
                return;
            }

            m_meshes = nextMeshes;

            setFramesAsDirty();
        }
    }
}