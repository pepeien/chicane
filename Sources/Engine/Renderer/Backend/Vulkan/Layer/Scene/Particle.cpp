#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Particle.hpp"

#include <algorithm>

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneParticle::VulkanLSceneParticle()
            : Layer(SCENE_PARTICLE_LAYER_ID),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLSceneParticle::onInit()
        {
            initFrameResources();
            initGraphicsPipeline();
            initFramebuffers();
        }

        void VulkanLSceneParticle::onRestart()
        {
            initFramebuffers();
        }

        void VulkanLSceneParticle::onDestruction()
        {
            destroyFrameResources();
            m_graphicsPipeline.destroy();
        }

        bool VulkanLSceneParticle::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasParticles();
        }

        void VulkanLSceneParticle::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanFrame&   frame   = *((VulkanFrame*)inData);

            vk::CommandBuffer commandBuffer = frame.commandBuffer;
            vk::Viewport      viewport      = backend->getVkViewport(this);
            commandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor = backend->getVkScissor(this);
            commandBuffer.setScissor(0, 1, &scissor);

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass               = m_graphicsPipeline.renderPass;
            beginInfo.framebuffer              = frame.image.getFramebuffer(m_id);
            beginInfo.renderArea.extent.width  = viewport.width;
            beginInfo.renderArea.extent.height = viewport.height;
            beginInfo.clearValueCount          = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues             = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

            m_graphicsPipeline.bind(commandBuffer);
            m_graphicsPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));

            const std::uint32_t count =
                std::min(static_cast<std::uint32_t>(inFrame.getParticles().size()), MAX_PARTICLES);
            commandBuffer.draw(4, count, 0, 0);

            commandBuffer.endRenderPass();
        }

        void VulkanLSceneParticle::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 2;

            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (VulkanFrame& frame : backend->frames)
            {
                vk::DescriptorSet descriptorSet;
                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    backend->logicalDevice,
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

                vk::WriteDescriptorSet particleWriteInfo;
                particleWriteInfo.dstSet          = descriptorSet;
                particleWriteInfo.dstBinding      = 1;
                particleWriteInfo.dstArrayElement = 0;
                particleWriteInfo.descriptorCount = 1;
                particleWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                particleWriteInfo.pBufferInfo     = &frame.particleResource.bufferInfo;
                frame.addWriteDescriptorSet(particleWriteInfo);
            }
        }

        void VulkanLSceneParticle::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneParticle::initGraphicsPipeline()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Particle.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Particle.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthBoundsTestEnable = false;
            depth.stencilTestEnable     = false;
            depth.depthWriteEnable      = false;
            depth.depthTestEnable       = true;
            depth.depthCompareOp        = vk::CompareOp::eLess;
            depth.minDepthBounds        = 0.0f;
            depth.maxDepthBounds        = 1.0f;

            vk::AttachmentDescription colorAttachment;
            colorAttachment.format        = backend->getSceneColorFormat();
            colorAttachment.samples       = vk::SampleCountFlagBits::e1;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            colorAttachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachment.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;

            vk::AttachmentReference colorReference;
            colorReference.attachment = 0;
            colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDependency colorSubpassDepedency;
            colorSubpassDepedency.srcSubpass    = 0;
            colorSubpassDepedency.dstSubpass    = VK_SUBPASS_EXTERNAL;
            colorSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            colorSubpassDepedency.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

            vk::AttachmentDescription depthAttachment;
            depthAttachment.format        = backend->swapchain.depthFormat;
            depthAttachment.samples       = vk::SampleCountFlagBits::e1;
            depthAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            depthAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            depthAttachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.finalLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::AttachmentReference depthReference;
            depthReference.attachment = 1;
            depthReference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::SubpassDependency depthSubpassDepedency;
            depthSubpassDepedency.srcSubpass    = 0;
            depthSubpassDepedency.dstSubpass    = VK_SUBPASS_EXTERNAL;
            depthSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eLateFragmentTests;
            depthSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eFragmentShader;
            depthSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            depthSubpassDepedency.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount    = 1;
            subpass.pColorAttachments       = &colorReference;
            subpass.pDepthStencilAttachment = &depthReference;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.depthClampEnable        = false;
            rasterization.rasterizerDiscardEnable = false;
            rasterization.depthBiasEnable         = false;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            vk::PipelineInputAssemblyStateCreateInfo assembly = VulkanGraphicsPipeline::createInputAssemblyState();
            assembly.topology                                 = vk::PrimitiveTopology::eTriangleStrip;

            VulkanGraphicsPipelineBuilder()
                .setInputAssembly(assembly)
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createAdditiveBlendAttachmentState())
                .addAttachment(colorAttachment)
                .addSubpassDependecy(colorSubpassDepedency)
                .setDepthStencil(depth)
                .addAttachment(depthAttachment)
                .addSubpassDependecy(depthSubpassDepedency)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLSceneParticle::initFramebuffers()
        {
            VulkanBackend* backend  = getBackend<VulkanBackend>();
            vk::Viewport   viewport = backend->getVkViewport(this);

            for (VulkanSwapchainImage& frame : backend->swapchain.images)
            {
                VulkanFrameBufferCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline.renderPass;
                createInfo.extent.width  = viewport.width;
                createInfo.extent.height = viewport.height;
                createInfo.attachments.push_back(frame.targetImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                frame.addBuffer(createInfo);
            }
        }
    }
}
