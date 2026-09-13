#include "Chicane/Renderer/Backend/Vulkan/Bloom/Pass.hpp"

#include <array>
#include <cstdint>

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Image.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static vk::ImageMemoryBarrier makeColorBarrier(
            vk::Image       inImage,
            vk::ImageLayout inOldLayout,
            vk::ImageLayout inNewLayout,
            vk::AccessFlags inSrcAccess,
            vk::AccessFlags inDstAccess
        )
        {
            vk::ImageMemoryBarrier barrier;
            barrier.oldLayout                       = inOldLayout;
            barrier.newLayout                       = inNewLayout;
            barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.image                           = inImage;
            barrier.srcAccessMask                   = inSrcAccess;
            barrier.dstAccessMask                   = inDstAccess;
            barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = 1;

            return barrier;
        }

        void VulkanBloomPass::init(VulkanBackend* inBackend)
        {
            m_backend = inBackend;
            initPipelines();
        }

        void VulkanBloomPass::destroy()
        {
            destroyPipelines();
            m_backend = nullptr;
        }

        void VulkanBloomPass::initPipelines()
        {
            VulkanBackend* backend = m_backend;

            VulkanDescriptorSetLayoutBidingsCreateInfo extractBindings;
            extractBindings.count = 1;
            extractBindings.indices.push_back(0);
            extractBindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            extractBindings.counts.push_back(1);
            extractBindings.stages.push_back(vk::ShaderStageFlagBits::eFragment);
            VulkanDescriptorSetLayout::init(m_extractDescriptor.setLayout, backend->logicalDevice, extractBindings);

            VulkanDescriptorSetLayoutBidingsCreateInfo blurBindings = extractBindings;
            VulkanDescriptorSetLayout::init(m_blurDescriptor.setLayout, backend->logicalDevice, blurBindings);

            VulkanDescriptorSetLayoutBidingsCreateInfo compositeBindings;
            compositeBindings.count = 2;
            compositeBindings.indices.push_back(0);
            compositeBindings.indices.push_back(1);
            compositeBindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            compositeBindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            compositeBindings.counts.push_back(1);
            compositeBindings.counts.push_back(1);
            compositeBindings.stages.push_back(vk::ShaderStageFlagBits::eFragment);
            compositeBindings.stages.push_back(vk::ShaderStageFlagBits::eFragment);
            VulkanDescriptorSetLayout::init(m_compositeDescriptor.setLayout, backend->logicalDevice, compositeBindings);

            VulkanDescriptorPoolCreateInfo poolCreateInfo;
            poolCreateInfo.maxSets = 8;
            poolCreateInfo.sizes.push_back({vk::DescriptorType::eCombinedImageSampler, 16});
            VulkanDescriptorPool::init(m_extractDescriptor.pool, backend->logicalDevice, poolCreateInfo);
            m_blurDescriptor.pool      = m_extractDescriptor.pool;
            m_compositeDescriptor.pool = m_extractDescriptor.pool;

            VulkanDescriptorSetLayout::allocate(
                m_extractDescriptor.set,
                backend->logicalDevice,
                m_extractDescriptor.setLayout,
                m_extractDescriptor.pool
            );
            VulkanDescriptorSetLayout::allocate(
                m_blurDescriptor.set,
                backend->logicalDevice,
                m_blurDescriptor.setLayout,
                m_extractDescriptor.pool
            );
            VulkanDescriptorSetLayout::allocate(
                m_compositeDescriptor.set,
                backend->logicalDevice,
                m_compositeDescriptor.setLayout,
                m_extractDescriptor.pool
            );

            initFullscreenPipeline(
                m_extract,
                "Assets/Engine/Shaders/Vulkan/Post/Extract.vfrag",
                {m_extractDescriptor.setLayout},
                backend->getSceneColorFormat(),
                false
            );
            initFullscreenPipeline(
                m_blur,
                "Assets/Engine/Shaders/Vulkan/Post/Blur.vfrag",
                {m_blurDescriptor.setLayout},
                backend->getSceneColorFormat(),
                true
            );

            vk::AttachmentDescription compositeAttachment;
            compositeAttachment.format        = backend->swapchain.colorFormat;
            compositeAttachment.samples       = vk::SampleCountFlagBits::e1;
            compositeAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            compositeAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            compositeAttachment.initialLayout = vk::ImageLayout::eUndefined;
            compositeAttachment.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Post/Fullscreen.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Assets/Engine/Shaders/Vulkan/Post/Composite.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            vk::AttachmentReference colorReference;
            colorReference.attachment = 0;
            colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDependency dependency;
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask =
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eFragmentShader;
            dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependency.srcAccessMask = vk::AccessFlagBits::eShaderRead;
            dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments    = &colorReference;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.polygonMode = vk::PolygonMode::eFill;
            rasterization.cullMode    = vk::CullModeFlagBits::eNone;
            rasterization.frontFace   = vk::FrontFace::eCounterClockwise;

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthTestEnable  = false;
            depth.depthWriteEnable = false;

            vk::Viewport viewport;
            viewport.width    = static_cast<float>(backend->swapchain.extent.width);
            viewport.height   = static_cast<float>(backend->swapchain.extent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vk::Rect2D scissor;
            scissor.extent = backend->swapchain.extent;

            vk::PushConstantRange hdrPush;
            hdrPush.stageFlags = vk::ShaderStageFlagBits::eFragment;
            hdrPush.offset     = 0;
            hdrPush.size       = sizeof(std::int32_t);

            VulkanGraphicsPipelineBuilder()
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(viewport)
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(scissor)
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState(false))
                .addAttachment(compositeAttachment)
                .addSubpassDependecy(dependency)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_compositeDescriptor.setLayout)
                .addPushConstant(hdrPush)
                .setDepthStencil(depth)
                .setRasterization(rasterization)
                .build(m_composite, backend->logicalDevice);
        }

        void VulkanBloomPass::destroyPipelines()
        {
            if (!m_backend)
            {
                return;
            }

            m_extract.destroy();
            m_blur.destroy();
            m_composite.destroy();

            m_backend->logicalDevice.destroyDescriptorSetLayout(m_extractDescriptor.setLayout);
            m_backend->logicalDevice.destroyDescriptorSetLayout(m_blurDescriptor.setLayout);
            m_backend->logicalDevice.destroyDescriptorSetLayout(m_compositeDescriptor.setLayout);
            m_backend->logicalDevice.destroyDescriptorPool(m_extractDescriptor.pool);
        }

        void VulkanBloomPass::rebuildFramebuffers()
        {
            if (!m_backend)
            {
                return;
            }

            for (VulkanSwapchainImage& image : m_backend->swapchain.images)
            {
                image.bloom.rebuildFramebuffers(
                    m_backend->logicalDevice,
                    m_extract.renderPass,
                    m_blur.renderPass,
                    m_composite.renderPass,
                    image.colorImage
                );
            }
        }

        void VulkanBloomPass::apply(VulkanFrame& inGpuFrame, const Frame& inFrame, bool bInShouldPresentToWindow)
        {
            if (!m_backend)
            {
                return;
            }

            VulkanSwapchainImage& image         = inGpuFrame.image;
            vk::CommandBuffer     commandBuffer = inGpuFrame.commandBuffer;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::PipelineStageFlagBits::eFragmentShader,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                makeColorBarrier(
                    image.targetImage.instance,
                    vk::ImageLayout::eColorAttachmentOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eColorAttachmentWrite,
                    vk::AccessFlagBits::eShaderRead
                )
            );

            if (!bInShouldPresentToWindow)
            {
                commandBuffer.pipelineBarrier(
                    vk::PipelineStageFlagBits::eTopOfPipe,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::DependencyFlags(),
                    nullptr,
                    nullptr,
                    makeColorBarrier(
                        image.colorImage.instance,
                        vk::ImageLayout::eUndefined,
                        vk::ImageLayout::eTransferDstOptimal,
                        vk::AccessFlagBits::eNone,
                        vk::AccessFlagBits::eTransferWrite
                    )
                );

                vk::ClearColorValue clear;
                clear.setFloat32({0.0f, 0.0f, 0.0f, 1.0f});
                vk::ImageSubresourceRange range;
                range.aspectMask = vk::ImageAspectFlagBits::eColor;
                range.levelCount = 1;
                range.layerCount = 1;
                commandBuffer
                    .clearColorImage(image.colorImage.instance, vk::ImageLayout::eTransferDstOptimal, clear, range);

                commandBuffer.pipelineBarrier(
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::DependencyFlags(),
                    nullptr,
                    nullptr,
                    makeColorBarrier(
                        image.colorImage.instance,
                        vk::ImageLayout::eTransferDstOptimal,
                        vk::ImageLayout::eColorAttachmentOptimal,
                        vk::AccessFlagBits::eTransferWrite,
                        vk::AccessFlagBits::eColorAttachmentWrite
                    )
                );

                return;
            }

            const bool bIsHDREnabled = inFrame.hasFeature(RendererFeature::HDR);

            updateSample(m_extractDescriptor.set, 0, image.targetImage);
            beginPass(commandBuffer, m_extract, image.bloom.extractFramebuffer, image.bloom.images.at(0).extent);
            m_extract.bind(commandBuffer);
            m_extract.bind(commandBuffer, 0, m_extractDescriptor.set);
            commandBuffer.draw(3, 1, 0, 0);
            commandBuffer.endRenderPass();

            blurPass(commandBuffer, image, 0, 1, 1.0f, 0.0f);
            blurPass(commandBuffer, image, 1, 0, 0.0f, 1.0f);
            blurPass(commandBuffer, image, 0, 1, 1.0f, 0.0f);
            blurPass(commandBuffer, image, 1, 0, 0.0f, 1.0f);

            updateSample(m_compositeDescriptor.set, 0, image.targetImage);
            updateSample(m_compositeDescriptor.set, 1, image.bloom.images.at(0));

            beginPass(commandBuffer, m_composite, image.bloom.compositeFramebuffer, image.colorImage.extent);
            m_composite.bind(commandBuffer);
            m_composite.bind(commandBuffer, 0, m_compositeDescriptor.set);
            const std::int32_t hdrEnabled = bIsHDREnabled ? 1 : 0;
            commandBuffer.pushConstants(
                m_composite.layout,
                vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(hdrEnabled),
                &hdrEnabled
            );
            commandBuffer.draw(3, 1, 0, 0);
            commandBuffer.endRenderPass();
        }

        void VulkanBloomPass::initFullscreenPipeline(
            VulkanGraphicsPipeline&                     outPipeline,
            const char*                                 inFragment,
            const std::vector<vk::DescriptorSetLayout>& inLayouts,
            vk::Format                                  inFormat,
            bool                                        bInHasPush
        )
        {
            VulkanBackend* backend = m_backend;

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Post/Fullscreen.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = inFragment;
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            vk::AttachmentDescription colorAttachment;
            colorAttachment.format        = inFormat;
            colorAttachment.samples       = vk::SampleCountFlagBits::e1;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eClear;
            colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
            colorAttachment.finalLayout   = vk::ImageLayout::eShaderReadOnlyOptimal;

            vk::AttachmentReference colorReference;
            colorReference.attachment = 0;
            colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDependency dependency;
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask =
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eFragmentShader;
            dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependency.srcAccessMask = vk::AccessFlagBits::eShaderRead;
            dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments    = &colorReference;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.polygonMode = vk::PolygonMode::eFill;
            rasterization.cullMode    = vk::CullModeFlagBits::eNone;
            rasterization.frontFace   = vk::FrontFace::eCounterClockwise;

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthTestEnable  = false;
            depth.depthWriteEnable = false;

            vk::Viewport viewport;
            viewport.width    = static_cast<float>(backend->swapchain.extent.width);
            viewport.height   = static_cast<float>(backend->swapchain.extent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vk::Rect2D scissor;
            scissor.extent = backend->swapchain.extent;

            VulkanGraphicsPipelineBuilder builder =
                VulkanGraphicsPipelineBuilder()
                    .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                    .addViewport(viewport)
                    .addDynamicState(vk::DynamicState::eViewport)
                    .addScissor(scissor)
                    .addDynamicState(vk::DynamicState::eScissor)
                    .addShaderStage(vertexShader, backend->logicalDevice)
                    .addShaderStage(fragmentShader, backend->logicalDevice)
                    .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState(false))
                    .addAttachment(colorAttachment)
                    .addSubpassDependecy(dependency)
                    .addSubpass(subpass)
                    .setDepthStencil(depth)
                    .setRasterization(rasterization);

            for (const vk::DescriptorSetLayout& layout : inLayouts)
            {
                builder = builder.addDescriptorSetLayout(layout);
            }

            if (bInHasPush)
            {
                vk::PushConstantRange range;
                range.stageFlags = vk::ShaderStageFlagBits::eFragment;
                range.offset     = 0;
                range.size       = sizeof(float) * 2;
                builder          = builder.addPushConstant(range);
            }

            builder.build(outPipeline, backend->logicalDevice);
        }

        void VulkanBloomPass::updateSample(
            vk::DescriptorSet inSet, std::uint32_t inBinding, const VulkanImageInfo& inImage
        ) const
        {
            vk::DescriptorImageInfo info;
            info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            info.imageView   = inImage.view;
            info.sampler     = inImage.sampler;

            vk::WriteDescriptorSet write;
            write.dstSet          = inSet;
            write.dstBinding      = inBinding;
            write.descriptorCount = 1;
            write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            write.pImageInfo      = &info;
            m_backend->logicalDevice.updateDescriptorSets(write, nullptr);
        }

        void VulkanBloomPass::beginPass(
            vk::CommandBuffer             inCommandBuffer,
            const VulkanGraphicsPipeline& inPipeline,
            vk::Framebuffer               inFramebuffer,
            vk::Extent2D                  inExtent
        ) const
        {
            vk::Viewport viewport;
            viewport.width    = static_cast<float>(inExtent.width);
            viewport.height   = static_cast<float>(inExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            inCommandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor;
            scissor.extent = inExtent;
            inCommandBuffer.setScissor(0, 1, &scissor);

            vk::ClearValue clear;
            clear.color = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass        = inPipeline.renderPass;
            beginInfo.framebuffer       = inFramebuffer;
            beginInfo.renderArea.extent = inExtent;
            beginInfo.clearValueCount   = 1;
            beginInfo.pClearValues      = &clear;
            inCommandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
        }

        void VulkanBloomPass::blurPass(
            vk::CommandBuffer     inCommandBuffer,
            VulkanSwapchainImage& inImage,
            int                   inSource,
            int                   inDestination,
            float                 inX,
            float                 inY
        )
        {
            updateSample(m_blurDescriptor.set, 0, inImage.bloom.images.at(inSource));
            beginPass(
                inCommandBuffer,
                m_blur,
                inImage.bloom.framebuffers.at(inDestination),
                inImage.bloom.images.at(inDestination).extent
            );
            m_blur.bind(inCommandBuffer);
            m_blur.bind(inCommandBuffer, 0, m_blurDescriptor.set);
            const std::array<float, 2> direction = {inX, inY};
            inCommandBuffer.pushConstants(
                m_blur.layout,
                vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(direction),
                direction.data()
            );
            inCommandBuffer.draw(3, 1, 0, 0);
            inCommandBuffer.endRenderPass();
        }
    }
}
