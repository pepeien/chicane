#include "Chicane/Runtime/Backend/Vulkan/Layer/Scene/Collision.hpp"

#include "Chicane/Runtime/Scene/Component/Mesh.hpp"
#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LSceneCollision::LSceneCollision()
            : Super("Engine_Scene_Collision"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_scene(nullptr),
            m_meshes({}),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
            m_clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

            loadEvents();
        }

        LSceneCollision::~LSceneCollision()
        {
            m_internals.logicalDevice.waitIdle();

            destroyModelData();

            m_graphicsPipeline.reset();
        }

        bool LSceneCollision::onInit()
        {
            if (!hasMeshes())
            {
                return false;
            }

            initFrameResources();

            initGraphicsPipeline();
            initFramebuffers();

            buildModelData();

            return true;
        }

        bool LSceneCollision::onDestroy()
        {
            if (is(RendererLayerStatus::Offline))
            {
                return false;
            }

            destroyFrameResources();

            return true;
        }

        bool LSceneCollision::onRebuild()
        {
            if (!hasMeshes())
            {
                return false;
            }

            initFrameResources();
            initFramebuffers();

            return true;
        }

        void LSceneCollision::onRender(void* outData)
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

                // Model
                renderModels(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void LSceneCollision::loadEvents()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            Application::watchScene(
                [&](Scene* inScene)
                {
                    m_scene = inScene;

                    if (m_scene == nullptr)
                    {
                        return;
                    }

                    m_scene->watchComponents(
                        [&](const std::vector<Component*>& inComponents)
                        {
                            m_meshes = m_scene->getComponents<CMesh>();

                            std::sort(
                                m_meshes.begin(),
                                m_meshes.end(),
                                [](CMesh* inA, CMesh* inB)
                                {
                                    return inA->getModel().compare(inB->getModel()) > 0;
                                }
                            );

                            if (is(RendererLayerStatus::Offline))
                            {
                                init();

                                return;
                            }

                            rebuildModelData();
                        }
                    );
                }
            );
        }

        bool LSceneCollision::hasMeshes() const
        {
            return !m_meshes.empty();
        }

        void LSceneCollision::initFrameResources()
        {
            if (!is(RendererLayerStatus::Offline) && !is(RendererLayerStatus::Initialized))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo bidings = {};
            bidings.count = 2;

            /// Camera
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            /// Mesh
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
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(
                m_internals.swapchain->frames.size()
            );
            descriptorPoolCreateInfo.sizes.push_back(
                { vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets }
            );
            descriptorPoolCreateInfo.sizes.push_back(
                { vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets }
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
        
                vk::WriteDescriptorSet cameraWriteInfo = {};
                cameraWriteInfo.dstSet          = descriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);

                vk::WriteDescriptorSet meshWriteInfo = {};
                meshWriteInfo.dstSet          = descriptorSet;
                meshWriteInfo.dstBinding      = 1;
                meshWriteInfo.dstArrayElement = 0;
                meshWriteInfo.descriptorCount = 1;
                meshWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                meshWriteInfo.pBufferInfo     = &frame.meshResource.bufferInfo;
                frame.addWriteDescriptorSet(meshWriteInfo);
            }
        }

        void LSceneCollision::destroyFrameResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            m_internals.logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void LSceneCollision::initGraphicsPipeline()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            // Shader
            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Collision.vert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Collision.frag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Shader::StageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);
            shaders.push_back(fragmentShader);

            // Set Layouts
            std::vector<vk::DescriptorSetLayout> setLayouts = {};
            setLayouts.push_back(m_frameDescriptor.setLayout);

            // Attachments
            GraphicsPipeline::Attachment colorAttachment = {};
            colorAttachment.type          = GraphicsPipeline::Attachment::Type::Color;
            colorAttachment.format        = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            GraphicsPipeline::Attachment depthAttachment = {};
            depthAttachment.type          = GraphicsPipeline::Attachment::Type::Depth;
            depthAttachment.format        = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            std::vector<GraphicsPipeline::Attachment> attachments = {};
            attachments.push_back(colorAttachment);
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
            createInfo.rasterizaterizationState = GraphicsPipeline::createRasterizationState(
                vk::PolygonMode::eLine
            );

            m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
        }

        void LSceneCollision::initFramebuffers()
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
                createInfo.attachments.push_back(frame.depthImage.view);

                Frame::Buffer::init(frame, createInfo);
            }
        }

        void LSceneCollision::buildModelVertexBuffer()
        {
            std::vector<Vertex> vertices = {};

            for (CMesh* mesh : m_meshes)
            {
                const Bounds& bounds = mesh->getBounds();

                for (const Vec3& boundVertex : bounds.getVertices())
                {
                    Vertex vertex = {};
                    vertex.position = boundVertex;

                    vertices.push_back(vertex);
                }
            }

            BufferCreateInfo createInfo = {};
            createInfo.physicalDevice   = m_internals.physicalDevice;
            createInfo.logicalDevice    = m_internals.logicalDevice;
            createInfo.size             = sizeof(Vertex) * vertices.size();
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
            memcpy(writeLocation, vertices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                          vk::BufferUsageFlagBits::eVertexBuffer;
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

        void LSceneCollision::buildModelIndexBuffer()
        {
            std::vector<std::uint32_t> indices = {};

            for (CMesh* mesh : m_meshes)
            {
                std::vector<std::uint32_t> boundIndices = mesh->getBounds().getIndices();

                indices.insert(
                    indices.end(),
                    boundIndices.begin(),
                    boundIndices.end()
                );
            }

            BufferCreateInfo createInfo = {};
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

        void LSceneCollision::buildModelData()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LSceneCollision::destroyModelData()
        {
            m_internals.logicalDevice.waitIdle();

            m_modelVertexBuffer.destroy(m_internals.logicalDevice);
            m_modelIndexBuffer.destroy(m_internals.logicalDevice);
        }

        void LSceneCollision::rebuildModelData()
        {
            destroyModelData();

            if (!hasMeshes())
            {
                return;
            }

            buildModelData();
        }

        void LSceneCollision::renderModels(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Buffer vertexBuffers[] = { m_modelVertexBuffer.instance };
            vk::DeviceSize offsets[]   = { 0 };

            inCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            inCommandBuffer.bindIndexBuffer(m_modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

            std::uint32_t firstIndex  = 0;
            std::uint32_t vertexIndex = 0;
            std::uint32_t instance    = 0;

            for (CMesh* mesh : m_meshes)
            {
                const Bounds&                     bounds   = mesh->getBounds();
                const std::vector<std::uint32_t>& indices  = bounds.getIndices();
                const std::vector<Vec3>&          vertices = bounds.getVertices();

                const std::uint32_t indexCount  = static_cast<std::uint32_t>(indices.size());
                const std::uint32_t vertexCount = static_cast<std::uint32_t>(vertices.size());

                inCommandBuffer.drawIndexed(
                    indexCount,
                    1,
                    firstIndex,
                    vertexIndex,
                    instance
                );

                firstIndex  += indexCount;
                vertexIndex += vertexCount;
                instance++;
            }
        }
    }
}