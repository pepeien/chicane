#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Sky.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneSky::VulkanLSceneSky()
            : Layer("Engine_Scene_Sky"),
              m_frameDescriptor({}),
              m_textureDescriptor({}),
              m_sky(nullptr),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f)})
        {}

        VulkanLSceneSky::~VulkanLSceneSky()
        {
            destroyTextureData();

            m_sky.reset();
        }

        bool VulkanLSceneSky::onInit()
        {
            initFrameResources();

            buildTextureDescriptor();

            initGraphicsPipeline();
            initFramebuffers();

            return true;
        }

        bool VulkanLSceneSky::onDestroy()
        {
            destroyFrameResources();

            return true;
        }

        bool VulkanLSceneSky::onRebuild()
        {
            initFrameResources();
            initFramebuffers();

            return true;
        }

        void VulkanLSceneSky::onLoad(const DrawSky& inResource)
        {
            if (inResource.textures.empty() || inResource.model.id == Draw::UnknownId)
            {
                return;
            }

            buildTextureData(inResource);
        }

        bool VulkanLSceneSky::onSetup(const Frame& inFrame)
        {
            if (inFrame.getSkyInstance().model.id == Draw::UnknownId || !m_sky)
            {
                return false;
            }

            return true;
        }

        void VulkanLSceneSky::onRender(const Frame& inFrame, void* inData)
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>();

            VulkanSwapchainImage image         = *((VulkanSwapchainImage*)inData);
            vk::CommandBuffer    commandBuffer = image.commandBuffer;

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass        = m_graphicsPipeline.renderPass;
            beginInfo.framebuffer       = image.getFramebuffer(m_id);
            beginInfo.renderArea.extent = backend->swapchain.extent;
            beginInfo.clearValueCount   = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues      = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
            // Pipeline
            m_graphicsPipeline.bind(commandBuffer);

            // Frame
            m_graphicsPipeline.bindDescriptorSet(commandBuffer, 0, image.getDescriptorSet(m_id));

            // Texture
            m_sky->bind(commandBuffer, m_graphicsPipeline.layout);

            // Draw
            vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
            vk::DeviceSize offsets[]       = {0};

            commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

            commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

            const DrawPoly& draw = inFrame.getSkyInstance().model;

            commandBuffer.drawIndexed(draw.indexCount, 1, draw.indexStart, draw.vertexStart, 0);

            commandBuffer.endRenderPass();
        }

        void VulkanLSceneSky::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 1;

            // Camera
            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->swapchain.images.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    backend->logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                image.addDescriptorSet(m_id, descriptorSet);

                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = descriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &image.cameraResource.bufferInfo;
                image.addWriteDescriptorSet(cameraWriteInfo);
            }
        }

        void VulkanLSceneSky::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneSky::initGraphicsPipeline()
        {
            // Backend
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>();

            // Shader
            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Sky.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Contents/Engine/Shaders/Vulkan/Scene/Sky.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            // Attachments
            vk::AttachmentDescription colorAttachment;
            colorAttachment.flags         = vk::AttachmentDescriptionFlags();
            colorAttachment.format        = backend->swapchain.colorFormat;
            colorAttachment.samples       = vk::SampleCountFlagBits::e1;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
            colorAttachment.finalLayout   = vk::ImageLayout::ePresentSrcKHR;

            vk::AttachmentReference colorReference;
            colorReference.attachment = 0;
            colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDependency colorSubpassDepedency;
            colorSubpassDepedency.srcSubpass    = VK_SUBPASS_EXTERNAL;
            colorSubpassDepedency.dstSubpass    = 0;
            colorSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            colorSubpassDepedency.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

            vk::SubpassDescription subpass;
            subpass.flags                = vk::SubpassDescriptionFlags();
            subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments    = &colorReference;

            // Rasterizer
            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.flags                   = vk::PipelineRasterizationStateCreateFlags();
            rasterization.depthClampEnable        = VK_FALSE;
            rasterization.rasterizerDiscardEnable = VK_FALSE;
            rasterization.lineWidth               = 1.0f;
            rasterization.depthBiasEnable         = VK_TRUE;
            rasterization.depthBiasClamp          = 0.0f;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eFront;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            // Build
            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->viewport)
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->scissor)
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState())
                .addAttachment(colorAttachment)
                .addSubpassDependecy(colorSubpassDepedency)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .addDescriptorSetLayout(m_textureDescriptor.setLayout)
                .setRasterization(rasterization)
                .build(m_graphicsPipeline, backend->logicalDevice);
        }

        void VulkanLSceneSky::initFramebuffers()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            for (VulkanSwapchainImage& image : backend->swapchain.images)
            {
                VulkanFrameCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_graphicsPipeline.renderPass;
                createInfo.extent        = backend->swapchain.extent;
                createInfo.attachments.push_back(image.colorImage.view);

                image.addBuffer(createInfo);
            }
        }

        void VulkanLSceneSky::buildTextureDescriptor()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;

            layoutBidings.indices.push_back(0);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(1);
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(m_textureDescriptor.setLayout, backend->logicalDevice, layoutBidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = 1;
            descriptorPoolCreateInfo.sizes.push_back({vk::DescriptorType::eCombinedImageSampler, 1});

            VulkanDescriptorPool::init(m_textureDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);
        }

        void VulkanLSceneSky::buildTextureData(const DrawSky& inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanSkyCreateInfo createInfo;
            createInfo.images              = {};
            createInfo.logicalDevice       = backend->logicalDevice;
            createInfo.physicalDevice      = backend->physicalDevice;
            createInfo.commandBuffer       = backend->mainCommandBuffer;
            createInfo.queue               = backend->graphicsQueue;
            createInfo.descriptorPool      = m_textureDescriptor.pool;
            createInfo.descriptorSetLayout = m_textureDescriptor.setLayout;

            for (const DrawTexture& texture : inData.textures)
            {
                createInfo.images.push_back(texture.image);
            }

            m_sky.reset();
            m_sky = std::make_unique<VulkanSky>(createInfo);
        }

        void VulkanLSceneSky::destroyTextureData()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_textureDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_textureDescriptor.pool);
        }
    }
}