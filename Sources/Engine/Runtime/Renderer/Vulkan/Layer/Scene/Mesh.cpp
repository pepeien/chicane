#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene/Mesh.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Renderer/Vulkan.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Data.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Buffer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Buffer/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        LSceneMesh::LSceneMesh()
            : Super("Engine_Scene_Mesh"),
              m_internals(Application::getRenderer<Renderer>()->getInternals()),
              m_clearValues({}),
              m_textureManager(Box::getTextureManager()),
              m_modelManager(Box::getModelManager())
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
            m_clearValues.push_back(vk::ClearDepthStencilValue(1.0f, 0));

            loadEvents();
        }

        LSceneMesh::~LSceneMesh()
        {
            m_internals.logicalDevice.waitIdle();

            destroyTextureResources();
            destroyModelData();

            m_graphicsPipeline.reset();
        }

        bool LSceneMesh::onInit()
        {
            if (m_modelManager->isEmpty() || m_textureManager->isEmpty())
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

        bool LSceneMesh::onDestroy()
        {
            if (is(RendererLayerStatus::Offline))
            {
                return false;
            }

            destroyFrameResources();

            return true;
        }

        bool LSceneMesh::onRebuild()
        {
            if (m_modelManager->isEmpty() || m_textureManager->isEmpty())
            {
                return false;
            }

            initFrameResources();
            initFramebuffers();

            return true;
        }

        void LSceneMesh::onRender(void* outData)
        {
            if (!is(RendererLayerStatus::Running))
            {
                return;
            }

            RendererData*      data          = (RendererData*)outData;
            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Frame::Instance&   frame         = data->frame;

            vk::RenderPassBeginInfo beginInfo = {};
            beginInfo.renderPass              = m_graphicsPipeline->renderPass;
            beginInfo.framebuffer             = frame.getFramebuffer(m_id);
            beginInfo.renderArea.offset.x     = 0;
            beginInfo.renderArea.offset.y     = 0;
            beginInfo.renderArea.extent       = data->swapChainExtent;
            beginInfo.clearValueCount         = static_cast<std::uint32_t>(m_clearValues.size());
            beginInfo.pClearValues            = m_clearValues.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
            // Pipeline
            m_graphicsPipeline->bind(commandBuffer);

            // Frame
            m_graphicsPipeline->bindDescriptorSet(commandBuffer, 0, frame.getDescriptorSet(m_id));

            // Texture
            m_graphicsPipeline->bindDescriptorSet(commandBuffer, 1, m_textureDescriptor.set);

            // Model
            renderModels(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void LSceneMesh::loadEvents()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            m_textureManager->watchChanges([&](Box::ManagerEventType inEvent) {
                if (inEvent != Box::ManagerEventType::Activation)
                {
                    return;
                }

                if (is(RendererLayerStatus::Offline))
                {
                    init();

                    return;
                }

                buildTextureData();
            });

            m_modelManager->watchChanges([&](Box::ManagerEventType inEvent) {
                if (inEvent != Box::ManagerEventType::Use)
                {
                    return;
                }

                if (is(RendererLayerStatus::Offline))
                {
                    init();

                    return;
                }

                rebuildModelData();
            });
        }

        void LSceneMesh::initFrameResources()
        {
            if (!is(RendererLayerStatus::Offline) && !is(RendererLayerStatus::Initialized))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo bidings = {};
            bidings.count                                  = 4;

            /// Camera
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            /// Light
            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            /// Model
            bidings.indices.push_back(2);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            // Shadow
            bidings.indices.push_back(3);
            bidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Descriptor::initSetLayout(m_frameDescriptor.setLayout, m_internals.logicalDevice, bidings);

            Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eCombinedImageSampler, descriptorPoolCreateInfo.maxSets}
            );

            Descriptor::initPool(m_frameDescriptor.pool, m_internals.logicalDevice, descriptorPoolCreateInfo);

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
                cameraWriteInfo.dstSet                 = descriptorSet;
                cameraWriteInfo.dstBinding             = 0;
                cameraWriteInfo.dstArrayElement        = 0;
                cameraWriteInfo.descriptorCount        = 1;
                cameraWriteInfo.descriptorType         = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo            = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);

                vk::WriteDescriptorSet lightWriteInfo = {};
                lightWriteInfo.dstSet                 = descriptorSet;
                lightWriteInfo.dstBinding             = 1;
                lightWriteInfo.dstArrayElement        = 0;
                lightWriteInfo.descriptorCount        = 1;
                lightWriteInfo.descriptorType         = vk::DescriptorType::eUniformBuffer;
                lightWriteInfo.pBufferInfo            = &frame.lightResource.bufferInfo;
                frame.addWriteDescriptorSet(lightWriteInfo);

                vk::WriteDescriptorSet meshWriteInfo = {};
                meshWriteInfo.dstSet                 = descriptorSet;
                meshWriteInfo.dstBinding             = 2;
                meshWriteInfo.dstArrayElement        = 0;
                meshWriteInfo.descriptorCount        = 1;
                meshWriteInfo.descriptorType         = vk::DescriptorType::eStorageBuffer;
                meshWriteInfo.pBufferInfo            = &frame.meshResource.bufferInfo;
                frame.addWriteDescriptorSet(meshWriteInfo);

                vk::WriteDescriptorSet shadowWriteInfo = {};
                shadowWriteInfo.dstSet                 = descriptorSet;
                shadowWriteInfo.dstBinding             = 3;
                shadowWriteInfo.dstArrayElement        = 0;
                shadowWriteInfo.descriptorCount        = 1;
                shadowWriteInfo.descriptorType         = vk::DescriptorType::eCombinedImageSampler;
                shadowWriteInfo.pImageInfo             = &frame.shadowImageInfo;
                frame.addWriteDescriptorSet(shadowWriteInfo);
            }
        }

        void LSceneMesh::destroyFrameResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            m_internals.logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void LSceneMesh::initGraphicsPipeline()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            // Shader
            Shader::StageCreateInfo vertexShader = {};
            vertexShader.path                    = "Contents/Engine/Shaders/Vulkan/Scene/Mesh.vert";
            vertexShader.type                    = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader = {};
            fragmentShader.path                    = "Contents/Engine/Shaders/Vulkan/Scene/Mesh.frag";
            fragmentShader.type                    = vk::ShaderStageFlagBits::eFragment;

            std::vector<Shader::StageCreateInfo> shaders = {};
            shaders.push_back(vertexShader);
            shaders.push_back(fragmentShader);

            // Set Layouts
            std::vector<vk::DescriptorSetLayout> setLayouts = {};
            setLayouts.push_back(m_frameDescriptor.setLayout);
            setLayouts.push_back(m_textureDescriptor.setLayout);

            // Attachments
            GraphicsPipeline::Attachment colorAttachment = {};
            colorAttachment.type                         = GraphicsPipeline::AttachmentType::Color;
            colorAttachment.format                       = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp                       = vk::AttachmentLoadOp::eLoad;
            colorAttachment.initialLayout                = vk::ImageLayout::ePresentSrcKHR;
            colorAttachment.finalLayout                  = vk::ImageLayout::ePresentSrcKHR;

            GraphicsPipeline::Attachment depthAttachment = {};
            depthAttachment.type                         = GraphicsPipeline::AttachmentType::Depth;
            depthAttachment.format                       = m_internals.swapchain->depthFormat;
            depthAttachment.loadOp                       = vk::AttachmentLoadOp::eClear;
            depthAttachment.initialLayout                = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout                  = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            std::vector<GraphicsPipeline::Attachment> attachments = {};
            attachments.push_back(colorAttachment);
            attachments.push_back(depthAttachment);

            GraphicsPipeline::CreateInfo createInfo = {};
            createInfo.bHasVertices                 = true;
            createInfo.bHasDepthWrite               = true;
            createInfo.bHasBlending                 = false;
            createInfo.logicalDevice                = m_internals.logicalDevice;
            createInfo.shaders                      = shaders;
            createInfo.extent                       = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts         = setLayouts;
            createInfo.attachments                  = attachments;
            createInfo.rasterizaterizationState = GraphicsPipeline::createRasterizationState(vk::PolygonMode::eFill);

            m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(createInfo);
        }

        void LSceneMesh::initFramebuffers()
        {
            if (!is(RendererLayerStatus::Offline) && !is(RendererLayerStatus::Initialized))
            {
                return;
            }

            for (Frame::Instance& frame : m_internals.swapchain->frames)
            {
                Frame::Buffer::CreateInfo createInfo = {};
                createInfo.id                        = m_id;
                createInfo.logicalDevice             = m_internals.logicalDevice;
                createInfo.renderPass                = m_graphicsPipeline->renderPass;
                createInfo.extent                    = m_internals.swapchain->extent;
                createInfo.attachments.push_back(frame.colorImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                Frame::Buffer::init(frame, createInfo);
            }
        }

        void LSceneMesh::initTextureResources()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            Descriptor::SetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;

            layoutBidings.indices.push_back(0);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(Box::Texture::MAX_COUNT);
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Descriptor::initSetLayout(m_textureDescriptor.setLayout, m_internals.logicalDevice, layoutBidings);

            Descriptor::PoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = 1;
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eCombinedImageSampler, Box::Texture::MAX_COUNT}
            );

            Descriptor::initPool(m_textureDescriptor.pool, m_internals.logicalDevice, descriptorPoolCreateInfo);

            Descriptor::allocalteSet(
                m_textureDescriptor.set,
                m_internals.logicalDevice,
                m_textureDescriptor.setLayout,
                m_textureDescriptor.pool
            );
        }

        void LSceneMesh::destroyTextureResources()
        {
            m_internals.logicalDevice.destroyDescriptorSetLayout(m_textureDescriptor.setLayout);
            m_internals.logicalDevice.destroyDescriptorPool(m_textureDescriptor.pool);
        }

        void LSceneMesh::buildTextureData()
        {
            if (m_textureManager->isEmpty())
            {
                return;
            }

            // Textures
            Texture::CreateInfo createInfo = {};
            createInfo.logicalDevice       = m_internals.logicalDevice;
            createInfo.physicalDevice      = m_internals.physicalDevice;
            createInfo.commandBuffer       = m_internals.mainCommandBuffer;
            createInfo.queue               = m_internals.graphicsQueue;

            std::vector<vk::DescriptorImageInfo> imageInfos = {};

            for (const String& id : m_textureManager->getActiveIds())
            {
                createInfo.image = m_textureManager->getData(id);

                m_textures[id] = std::make_unique<Texture::Instance>(createInfo);

                Image::Data image = m_textures.at(id)->getImage();

                vk::DescriptorImageInfo info = {};
                info.imageLayout             = vk::ImageLayout::eShaderReadOnlyOptimal;
                info.imageView               = image.view;
                info.sampler                 = image.sampler;

                imageInfos.push_back(info);
            }

            vk::WriteDescriptorSet set = {};
            set.dstSet                 = m_textureDescriptor.set;
            set.dstBinding             = 0;
            set.dstArrayElement        = 0;
            set.descriptorCount        = static_cast<std::uint32_t>(imageInfos.size());
            set.descriptorType         = vk::DescriptorType::eCombinedImageSampler;
            set.pImageInfo             = imageInfos.data();

            m_internals.logicalDevice.updateDescriptorSets(set, nullptr);
        }

        void LSceneMesh::buildModelVertexBuffer()
        {
            const auto& vertices = m_modelManager->getVertices();

            BufferCreateInfo createInfo;
            createInfo.physicalDevice = m_internals.physicalDevice;
            createInfo.logicalDevice  = m_internals.logicalDevice;
            createInfo.size           = sizeof(Chicane::Vertex) * vertices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, vertices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_modelVertexBuffer.init(createInfo);

            stagingBuffer
                .copy(m_modelVertexBuffer, createInfo.size, m_internals.graphicsQueue, m_internals.mainCommandBuffer);
            stagingBuffer.destroy(m_internals.logicalDevice);
        }

        void LSceneMesh::buildModelIndexBuffer()
        {
            const auto& indices = m_modelManager->getIndices();

            BufferCreateInfo createInfo;
            createInfo.physicalDevice = m_internals.physicalDevice;
            createInfo.logicalDevice  = m_internals.logicalDevice;
            createInfo.size           = sizeof(std::uint32_t) * indices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            Buffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = m_internals.logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, indices.data(), createInfo.size);
            m_internals.logicalDevice.unmapMemory(stagingBuffer.memory);

            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            m_modelIndexBuffer.init(createInfo);

            stagingBuffer
                .copy(m_modelIndexBuffer, createInfo.size, m_internals.graphicsQueue, m_internals.mainCommandBuffer);
            stagingBuffer.destroy(m_internals.logicalDevice);
        }

        void LSceneMesh::buildModelData()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LSceneMesh::destroyModelData()
        {
            m_internals.logicalDevice.waitIdle();

            m_modelVertexBuffer.destroy(m_internals.logicalDevice);
            m_modelIndexBuffer.destroy(m_internals.logicalDevice);
        }

        void LSceneMesh::rebuildModelData()
        {
            destroyModelData();

            if (m_modelManager->isEmpty())
            {
                return;
            }

            buildModelData();
        }

        void LSceneMesh::renderModels(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Buffer     vertexBuffers[] = {m_modelVertexBuffer.instance};
            vk::DeviceSize offsets[]       = {0};

            inCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            inCommandBuffer.bindIndexBuffer(m_modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

            for (const String& id : m_modelManager->getActiveIds())
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