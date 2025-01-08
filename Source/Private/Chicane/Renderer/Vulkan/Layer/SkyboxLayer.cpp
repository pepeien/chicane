#include "Chicane/Renderer/Vulkan/Layer/SkyboxLayer.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        SkyboxLayer::SkyboxLayer()
            : Layer("Skybox"),
            m_internals(Application::getRenderer<Vulkan::Renderer>()->getInternals()),
            m_cubeMapManager(Loader::getCubeMapManager()),
            m_clearValues({})
        {
            m_clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

            loadEvents();
        }

        SkyboxLayer::~SkyboxLayer()
        {
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
            if (!is(Status::Initialized))
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

            setStatus(Status::Running);
        }

        void SkyboxLayer::destroy()
        {
            if (!is(Status::Running))
            {
                return;
            }

            m_internals.logicalDevice.destroyDescriptorPool(
                m_frameDescriptor.pool
            );

            setStatus(Status::Idle);
        }

        void SkyboxLayer::rebuild()
        {
            if (!is(Status::Idle))
            {
                return;
            }

            initFramebuffers();
            initFrameResources();

            setStatus(Status::Running);
        }

        void SkyboxLayer::render(void* outData)
        {
            if (!is(Status::Running))
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
                renderCubeMap(commandBuffer);
            commandBuffer.endRenderPass();
        }

        void SkyboxLayer::loadEvents()
        {
            if (!is(Status::Idle))
            {
                return;
            }

            m_cubeMapManager->watchChanges(
                [this](ManagerEventType inEvent) {
                    if (inEvent != ManagerEventType::Activation)
                    {
                        return;
                    }

                    if (is(Status::Idle))
                    {
                        setStatus(Status::Initialized);

                        build();
                    }
                }
            );
        }

        void SkyboxLayer::initFrameDescriptorSetLayout()
        {
            if (!is(Status::Initialized))
            {
                return;
            }

            Vulkan::Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
            frameLayoutBidings.count = 1;

            /// Camera
            frameLayoutBidings.indices.push_back(0);
            frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            frameLayoutBidings.counts.push_back(1);
            frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            Vulkan::Descriptor::initSetLayout(
                m_frameDescriptor.setLayout,
                m_internals.logicalDevice,
                frameLayoutBidings
            );
        }

        void SkyboxLayer::initMaterialDescriptorSetLayout()
        {
            if (!is(Status::Initialized))
            {
                return;
            }

            Vulkan::Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
            materialLayoutBidings.count = 1;
            materialLayoutBidings.indices.push_back(0);
            materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            materialLayoutBidings.counts.push_back(1);
            materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            Vulkan::Descriptor::initSetLayout(
                m_materialDescriptor.setLayout,
                m_internals.logicalDevice,
                materialLayoutBidings
            );
        }

        void SkyboxLayer::initGraphicsPipeline()
        {
            if (!is(Status::Initialized))
            {
                return;
            }

            Vulkan::GraphicsPipeline::Attachment colorAttachment {};
            colorAttachment.format        = m_internals.swapchain->format;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            colorAttachment.initialLayout = vk::ImageLayout::eUndefined;

            Vulkan::GraphicsPipeline::CreateInfo createInfo {};
            createInfo.bHasVertices         = false;
            createInfo.bHasDepth            = false;
            createInfo.logicalDevice        = m_internals.logicalDevice;
            createInfo.vertexShaderPath     = "Content/Engine/Shaders/sky.vert.spv";
            createInfo.fragmentShaderPath   = "Content/Engine/Shaders/sky.frag.spv";
            createInfo.extent               = m_internals.swapchain->extent;
            createInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout, m_materialDescriptor.setLayout };
            createInfo.colorAttachment      = colorAttachment;
            createInfo.polygonMode          = vk::PolygonMode::eFill;

            m_graphicsPipeline = std::make_unique<Vulkan::GraphicsPipeline::Instance>(createInfo);
        }

        void SkyboxLayer::initFramebuffers()
        {
            if (is(Status::Running))
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

                Vulkan::Frame::Buffer::init(frame, framebufferCreateInfo);
            }
        }

        void SkyboxLayer::initFrameResources()
        {
            if (is(Status::Running))
            {
                return;
            }

            Vulkan::Descriptor::PoolCreateInfo frameDescriptorPoolCreateInfo;
            frameDescriptorPoolCreateInfo.size = static_cast<std::uint32_t>(m_internals.swapchain->frames.size());
            frameDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

            Vulkan::Descriptor::initPool(
                m_frameDescriptor.pool,
                m_internals.logicalDevice,
                frameDescriptorPoolCreateInfo
            );

            for (Vulkan::Frame::Instance& frame : m_internals.swapchain->frames)
            {
                vk::DescriptorSet descriptorSet;
                Vulkan::Descriptor::allocalteSet(
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
                writeDescriptorSet.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
                frame.addWriteDescriptorSet(writeDescriptorSet);
            }
        }

        void SkyboxLayer::initMaterialResources()
        {
            if (!is(Status::Initialized))
            {
                return;
            }

            Vulkan::Descriptor::PoolCreateInfo materialDescriptorPoolCreateInfo;
            materialDescriptorPoolCreateInfo.size  = 1;
            materialDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

            Vulkan::Descriptor::initPool(
                m_materialDescriptor.pool,
                m_internals.logicalDevice,
                materialDescriptorPoolCreateInfo
            );
        }

        void SkyboxLayer::buildCubeMap()
        {
            if (!is(Status::Initialized))
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

            inCommandBuffer.draw(CUBEMAP_IMAGE_COUNT, 1, 0, 0);
        }
    }
}