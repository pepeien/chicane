#include "Chicane/Renderer/Vulkan/Layer/SkyboxLayer.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        SkyboxLayer::SkyboxLayer()
            : Layer::Instance("Skybox"),
            m_internals(Application::getRenderer<Renderer>()->getInternals()),
            m_cubeMapManager(Loader::getCubeMapManager()),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

            loadEvents();
        }

        SkyboxLayer::~SkyboxLayer()
        {
            if (is(Layer::Status::Offline))
            {
                return;
            }

            m_internals.logicalDevice.waitIdle();

            // Graphics Pipeline
            m_graphicsPipeline.reset();

            // Descriptors
            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_frameDescriptor.setLayout
            );

            m_internals.logicalDevice.destroyDescriptorSetLayout(
                m_materialDescriptor.setLayout
            );
            m_internals.logicalDevice.destroyDescriptorPool(
                m_materialDescriptor.pool
            );
        }

        void SkyboxLayer::build()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            initFrameDescriptorSetLayout();
            initMaterialDescriptorSetLayout();
            initGraphicsPipeline();
            initFramebuffers();
            initFrameResources();
            initMaterialResources();

            buildCubeMap();

            setStatus(Layer::Status::Running);
        }

        void SkyboxLayer::destroy()
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            m_internals.logicalDevice.destroyDescriptorPool(
                m_frameDescriptor.pool
            );

            setStatus(Layer::Status::Idle);
        }

        void SkyboxLayer::rebuild()
        {
            if (!is(Layer::Status::Idle))
            {
                return;
            }

            initFramebuffers();
            initFrameResources();

            setStatus(Layer::Status::Running);
        }

        void SkyboxLayer::render(void* outData)
        {
            if (!is(Layer::Status::Running))
            {
                return;
            }

            Renderer::Data* data             = (Renderer::Data*) outData;
            vk::CommandBuffer& commandBuffer = data->commandBuffer;
            Frame::Instance& frame           = data->frame;

            vk::RenderPassBeginInfo beginInfo {};
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

                // Cube Map
                renderCubeMap(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void SkyboxLayer::loadEvents()
        {
            if (!is(Layer::Status::Offline))
            {
                return;
            }

            setStatus(Layer::Status::Idle);

            m_cubeMapManager->watchChanges(
                [this](Manager::EventType inEvent) {
                    if (inEvent != Manager::EventType::Activation)
                    {
                        return;
                    }

                    if (is(Layer::Status::Idle))
                    {
                        setStatus(Layer::Status::Initialized);

                        build();
                    }
                }
            );
        }

        void SkyboxLayer::initFrameDescriptorSetLayout()
        {
            if (!is(Layer::Status::Initialized))
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
        }

        void SkyboxLayer::initMaterialDescriptorSetLayout()
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
                m_materialDescriptor.setLayout,
                m_internals.logicalDevice,
                materialLayoutBidings
            );
        }

        void SkyboxLayer::initGraphicsPipeline()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Shader::StageCreateInfo vertexShader {};
            vertexShader.path = "Content/Engine/Vulkan/Shaders/sky.vert.spv";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            Shader::StageCreateInfo fragmentShader {};
            fragmentShader.path = "Content/Engine/Vulkan/Shaders/sky.frag.spv";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            std::vector<Shader::StageCreateInfo> shaders {};
            shaders.push_back(vertexShader);
            shaders.push_back(fragmentShader);

            std::vector<vk::DescriptorSetLayout> descriptorSetLayouts {};
            descriptorSetLayouts.push_back(m_frameDescriptor.setLayout);
            descriptorSetLayouts.push_back(m_materialDescriptor.setLayout);

            GraphicsPipeline::Attachment colorAttachment {};
            colorAttachment.type          = GraphicsPipeline::Attachment::Type::Color;
            colorAttachment.format        = m_internals.swapchain->colorFormat;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            std::vector<GraphicsPipeline::Attachment> attachments {};
            attachments.push_back(colorAttachment);

            GraphicsPipeline::CreateInfo createInfo {};
            createInfo.bHasVertices             = false;
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

        void SkyboxLayer::initFramebuffers()
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
                framebufferCreateInfo.attachments.push_back(frame.colorImage.view);

                Frame::Buffer::init(frame, framebufferCreateInfo);
            }
        }

        void SkyboxLayer::initFrameResources()
        {
            if (is(Layer::Status::Running))
            {
                return;
            }

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

        void SkyboxLayer::initMaterialResources()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            Descriptor::PoolCreateInfo materialDescriptorPoolCreateInfo;
            materialDescriptorPoolCreateInfo.maxSets = 1;
            materialDescriptorPoolCreateInfo.sizes.push_back(
                { .type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1 }
            );

            Descriptor::initPool(
                m_materialDescriptor.pool,
                m_internals.logicalDevice,
                materialDescriptorPoolCreateInfo
            );
        }

        void SkyboxLayer::buildCubeMap()
        {
            if (!is(Layer::Status::Initialized))
            {
                return;
            }

            if (m_cubeMap.get() != nullptr)
            {
                m_cubeMap.reset();
            }

            CubeMap::CreateInfo createInfo {};
            createInfo.images = m_cubeMapManager->getData(
                m_cubeMapManager->getActiveIds().back()
            );
            createInfo.logicalDevice       = m_internals.logicalDevice;
            createInfo.physicalDevice      = m_internals.physicalDevice;
            createInfo.commandBuffer       = m_internals.mainCommandBuffer;
            createInfo.queue               = m_internals.graphicsQueue;
            createInfo.descriptorPool      = m_materialDescriptor.pool;
            createInfo.descriptorSetLayout = m_materialDescriptor.setLayout;

            m_cubeMap = std::make_unique<CubeMap::Instance>(createInfo);
        }

        void SkyboxLayer::renderCubeMap(const vk::CommandBuffer& inCommandBuffer)
        {
            m_cubeMap->bind(
                inCommandBuffer,
                m_graphicsPipeline->layout
            );

            inCommandBuffer.draw(Chicane::CubeMap::IMAGE_COUNT, 1, 0, 0);
        }
    }
}