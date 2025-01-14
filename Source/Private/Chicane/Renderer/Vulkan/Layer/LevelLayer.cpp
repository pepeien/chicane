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
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
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
            if (is(Layer::Status::Offline))
            {
                return;
            }

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

            Frame::Instance* outFrame = (Frame::Instance*) outData;
            outFrame->updateMeshData(m_meshes);
        }

        void LevelLayer::render(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Renderer::Data* data = (Renderer::Data*) outData;

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
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            setStatus(Layer::Status::Idle);

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

            Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
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

            Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                frameLayoutBidings
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
        
                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = descriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);

                vk::WriteDescriptorSet meshWriteInfo;
                meshWriteInfo.dstSet          = descriptorSet;
                meshWriteInfo.dstBinding      = 1;
                meshWriteInfo.dstArrayElement = 0;
                meshWriteInfo.descriptorCount = 1;
                meshWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                meshWriteInfo.pBufferInfo     = &frame.meshResource.bufferInfo;
                frame.addWriteDescriptorSet(meshWriteInfo);
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

        std::vector<Shader::StageCreateInfo> LevelLayer::getGraphicsPipelineShaders()
        {
            Shader::StageCreateInfo vertexShader {};
            vertexShader.path = "Content/Engine/Vulkan/Shaders/level.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader {};
            fragmentShader.path = "Content/Engine/Vulkan/Shaders/level.frag.spv";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Shader::StageCreateInfo> result {};
            result.push_back(vertexShader);
            result.push_back(fragmentShader);

            return result;    
        }

        std::vector<vk::DescriptorSetLayout> LevelLayer::getGraphicsPipelineDescriptorLayouts()
        {
            std::vector<vk::DescriptorSetLayout> result {};
            result.push_back(m_frameDescriptor.setLayout);
            result.push_back(m_textureDescriptor.setLayout);

            return result;
        }

        std::vector<vk::AttachmentDescription> LevelLayer::getGraphicsPipelineAttachments()
        {
            GraphicsPipeline::Attachment colorAttachment {};
            colorAttachment.format        = m_internals.swapchain->format;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            GraphicsPipeline::Attachment depthAttachment {};
            depthAttachment.format        = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            std::vector<vk::AttachmentDescription> result {};
            result.push_back(GraphicsPipeline::createColorAttachment(colorAttachment));
            result.push_back(GraphicsPipeline::createDepthAttachment(depthAttachment));

            return result;
        }

        void LevelLayer::initGraphicsPipeline()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            GraphicsPipeline::CreateInfo createInfo {};
            createInfo.bHasVertices         = true;
            createInfo.bHasDepthWrite       = true;
            createInfo.bHasBlending         = true;
            createInfo.logicalDevice        = m_internals.logicalDevice;
            createInfo.shaders              = getGraphicsPipelineShaders();
            createInfo.extent               = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts = getGraphicsPipelineDescriptorLayouts();
            createInfo.attachments          = getGraphicsPipelineAttachments();
            createInfo.polygonMode          = vk::PolygonMode::eFill;

            m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
        }

        void LevelLayer::initFramebuffers()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

            for (Frame::Instance& frame : m_internals.swapchain->frames)
            {
                Frame::Buffer::CreateInfo framebufferCreateInfo {};
                framebufferCreateInfo.id              = m_id;
                framebufferCreateInfo.logicalDevice   = m_internals.logicalDevice;
                framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
                framebufferCreateInfo.swapChainExtent = m_internals.swapchain->extent;
                framebufferCreateInfo.attachments.push_back(frame.imageView);
                framebufferCreateInfo.attachments.push_back(frame.depth.imageView);

                Frame::Buffer::init(frame, framebufferCreateInfo);
            }
        }

        void LevelLayer::initTextureResources()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;
            layoutBidings.indices.push_back(0);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(Chicane::Texture::MAX_COUNT);
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Descriptor::initSetLayout(
                m_textureDescriptor.setLayout,
                m_internals.logicalDevice,
                layoutBidings
            );

            Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = 1;
            descriptorPoolCreateInfo.sizes.push_back(
                { .type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = Chicane::Texture::MAX_COUNT }
            );

            Descriptor::initPool(
                m_textureDescriptor.pool,
                m_internals.logicalDevice,
                descriptorPoolCreateInfo
            ); 

            Descriptor::allocalteSet(
                m_textureDescriptor.set,
                m_internals.logicalDevice,
                m_textureDescriptor.setLayout,
                m_textureDescriptor.pool
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

            vk::WriteDescriptorSet descriptorSet {};
            descriptorSet.dstSet          = m_textureDescriptor.set;
            descriptorSet.dstBinding      = 0;
            descriptorSet.dstArrayElement = 0;
            descriptorSet.descriptorCount = imageInfos.size();
            descriptorSet.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            descriptorSet.pImageInfo      = imageInfos.data();

            m_internals.logicalDevice.updateDescriptorSets(descriptorSet, nullptr);
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

            Buffer::destroy(
                m_internals.logicalDevice,
                m_modelVertexBuffer
            );
            Buffer::destroy(
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

            for (Frame::Instance& frame : m_internals.swapchain->frames)
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

            for (Frame::Instance& frame : m_internals.swapchain->frames)
            {
                frame.meshResource.setAsDirty();
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