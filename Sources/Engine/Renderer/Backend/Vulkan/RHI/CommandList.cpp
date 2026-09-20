#include "Backend/Vulkan/RHI/CommandList.hpp"

#include "Backend/Vulkan.hpp"
#include "Backend/Vulkan/Image.hpp"

#include <algorithm>
#include <array>

namespace Chicane
{
    namespace Renderer
    {
        VulkanRHICommandList::VulkanRHICommandList(VulkanRHIDevice* inDevice)
            : m_device(inDevice)
        {}

        VulkanRHICommandList::~VulkanRHICommandList()
        {
            for (auto& entry : m_retiredPasses)
            {
                destroyPassObjects(entry.second);
            }
        }

        std::uint64_t VulkanRHICommandList::commandKey(vk::CommandBuffer inCommandBuffer) const
        {
            return static_cast<std::uint64_t>(
                reinterpret_cast<std::uintptr_t>(static_cast<VkCommandBuffer>(inCommandBuffer))
            );
        }

        void VulkanRHICommandList::destroyPassObjects(std::vector<VulkanRHIPassObjects>& inObjects)
        {
            vk::Device device = m_device->backend()->logicalDevice;
            for (VulkanRHIPassObjects& objects : inObjects)
            {
                if (objects.framebuffer)
                {
                    device.destroyFramebuffer(objects.framebuffer);
                }
                if (objects.pass)
                {
                    device.destroyRenderPass(objects.pass);
                }
            }
            inObjects.clear();
        }

        void VulkanRHICommandList::setCommandBuffer(vk::CommandBuffer inCommandBuffer)
        {
            destroyPassObjects(m_retiredPasses[commandKey(inCommandBuffer)]);
            m_commands = inCommandBuffer;
        }

        static vk::AttachmentLoadOp toLoad(RHI::LoadOp inOp)
        {
            if (inOp == RHI::LoadOp::Clear)
            {
                return vk::AttachmentLoadOp::eClear;
            }
            if (inOp == RHI::LoadOp::DontCare)
            {
                return vk::AttachmentLoadOp::eDontCare;
            }
            return vk::AttachmentLoadOp::eLoad;
        }

        static VulkanRHIImageData* trackedImage(VulkanRHIImageData* inImage)
        {
            return inImage && inImage->parent ? inImage->parent : inImage;
        }

        static vk::ImageLayout attachmentLayout(const VulkanRHIImageData* inImage)
        {
            if (inImage->bHasDepth || inImage->kind == RHI::ImageKind::Depth2D ||
                inImage->kind == RHI::ImageKind::Depth2DArray)
            {
                return vk::ImageLayout::eDepthStencilAttachmentOptimal;
            }

            return vk::ImageLayout::eColorAttachmentOptimal;
        }

        static vk::ImageLayout shaderReadLayout(const VulkanRHIImageData* inImage)
        {
            if (inImage->bHasDepth || inImage->kind == RHI::ImageKind::Depth2D ||
                inImage->kind == RHI::ImageKind::Depth2DArray)
            {
                return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
            }

            return vk::ImageLayout::eShaderReadOnlyOptimal;
        }

        static vk::ImageAspectFlags imageAspect(const VulkanRHIImageData* inImage)
        {
            if (inImage->bHasDepth || inImage->kind == RHI::ImageKind::Depth2D ||
                inImage->kind == RHI::ImageKind::Depth2DArray)
            {
                return VulkanImage::depthAspect(inImage->info.format);
            }

            return vk::ImageAspectFlagBits::eColor;
        }

        static void layoutAccess(vk::ImageLayout inLayout, vk::AccessFlags& outAccess, vk::PipelineStageFlags& outStage)
        {
            switch (inLayout)
            {
            case vk::ImageLayout::eUndefined:
                outAccess = vk::AccessFlagBits::eNone;
                outStage  = vk::PipelineStageFlagBits::eTopOfPipe;
                break;
            case vk::ImageLayout::eColorAttachmentOptimal:
                outAccess = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
                outStage  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                break;
            case vk::ImageLayout::eDepthStencilAttachmentOptimal:
                outAccess =
                    vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                outStage =
                    vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
                break;
            case vk::ImageLayout::eShaderReadOnlyOptimal:
            case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
                outAccess = vk::AccessFlagBits::eShaderRead;
                outStage  = vk::PipelineStageFlagBits::eFragmentShader;
                break;
            case vk::ImageLayout::eTransferSrcOptimal:
                outAccess = vk::AccessFlagBits::eTransferRead;
                outStage  = vk::PipelineStageFlagBits::eTransfer;
                break;
            case vk::ImageLayout::eTransferDstOptimal:
                outAccess = vk::AccessFlagBits::eTransferWrite;
                outStage  = vk::PipelineStageFlagBits::eTransfer;
                break;
            case vk::ImageLayout::ePresentSrcKHR:
                outAccess = vk::AccessFlagBits::eNone;
                outStage  = vk::PipelineStageFlagBits::eBottomOfPipe;
                break;
            default:
                outAccess = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
                outStage  = vk::PipelineStageFlagBits::eAllCommands;
                break;
            }
        }

        void VulkanRHICommandList::setLayout(VulkanRHIImageData* inImage, vk::ImageLayout inLayout)
        {
            VulkanRHIImageData* image = trackedImage(inImage);
            if (!image || !m_commands)
            {
                return;
            }

            const vk::ImageLayout oldLayout = m_device->rememberedLayout(image);
            if (oldLayout == inLayout)
            {
                return;
            }

            vk::AccessFlags        srcAccess;
            vk::AccessFlags        dstAccess;
            vk::PipelineStageFlags srcStage;
            vk::PipelineStageFlags dstStage;
            layoutAccess(oldLayout, srcAccess, srcStage);
            layoutAccess(inLayout, dstAccess, dstStage);

            vk::ImageMemoryBarrier barrier;
            barrier.oldLayout                       = oldLayout;
            barrier.newLayout                       = inLayout;
            barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            barrier.image                           = image->info.instance;
            barrier.srcAccessMask                   = srcAccess;
            barrier.dstAccessMask                   = dstAccess;
            barrier.subresourceRange.aspectMask     = imageAspect(image);
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = std::max(1u, image->mips);
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount =
                image->kind == RHI::ImageKind::Cube ? 6u : std::max(1u, image->layers);

            m_commands.pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, barrier);
            m_device->rememberLayout(image, inLayout);
        }

        vk::RenderPass VulkanRHICommandList::framebufferPass(const RHI::PassCreateInfo& inCreateInfo)
        {
            std::vector<vk::AttachmentDescription> attachments;
            vk::AttachmentReference                colorRef;
            vk::AttachmentReference                depthRef;

            if (inCreateInfo.bHasColor)
            {
                auto*                     image = static_cast<VulkanRHIImageData*>(inCreateInfo.color.image.handle);
                vk::AttachmentDescription color;
                color.format        = image->info.format != vk::Format::eUndefined ? image->info.format
                                                                                   : m_device->toVkFormat(image->format);
                color.samples       = vk::SampleCountFlagBits::e1;
                color.loadOp        = toLoad(inCreateInfo.color.load);
                color.storeOp       = vk::AttachmentStoreOp::eStore;
                color.initialLayout = inCreateInfo.color.load == RHI::LoadOp::Clear
                                          ? vk::ImageLayout::eUndefined
                                          : vk::ImageLayout::eColorAttachmentOptimal;
                color.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;
                colorRef.attachment = 0;
                colorRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;
                attachments.push_back(color);
            }

            if (inCreateInfo.bHasDepth)
            {
                auto*                     image = static_cast<VulkanRHIImageData*>(inCreateInfo.depth.image.handle);
                vk::AttachmentDescription depth;
                depth.format         = image->info.format != vk::Format::eUndefined ? image->info.format
                                                                                    : m_device->toVkFormat(image->format);
                depth.samples        = vk::SampleCountFlagBits::e1;
                depth.loadOp         = toLoad(inCreateInfo.depth.load);
                depth.storeOp        = vk::AttachmentStoreOp::eStore;
                depth.stencilLoadOp  = toLoad(inCreateInfo.depth.load);
                depth.stencilStoreOp = vk::AttachmentStoreOp::eStore;
                depth.initialLayout  = inCreateInfo.depth.load == RHI::LoadOp::Clear
                                           ? vk::ImageLayout::eUndefined
                                           : vk::ImageLayout::eDepthStencilAttachmentOptimal;
                depth.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                depthRef.attachment  = static_cast<std::uint32_t>(attachments.size());
                depthRef.layout      = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                attachments.push_back(depth);
            }

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            if (inCreateInfo.bHasColor)
            {
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments    = &colorRef;
            }
            if (inCreateInfo.bHasDepth)
            {
                subpass.pDepthStencilAttachment = &depthRef;
            }

            vk::RenderPassCreateInfo info;
            info.attachmentCount = static_cast<std::uint32_t>(attachments.size());
            info.pAttachments    = attachments.data();
            info.subpassCount    = 1;
            info.pSubpasses      = &subpass;

            return m_device->backend()->logicalDevice.createRenderPass(info);
        }

        vk::Framebuffer VulkanRHICommandList::framebuffer(
            const RHI::PassCreateInfo& inCreateInfo, vk::RenderPass inPass
        )
        {
            std::vector<vk::ImageView> views;
            if (inCreateInfo.bHasColor)
            {
                views.push_back(static_cast<VulkanRHIImageData*>(inCreateInfo.color.image.handle)->info.view);
            }
            if (inCreateInfo.bHasDepth)
            {
                views.push_back(static_cast<VulkanRHIImageData*>(inCreateInfo.depth.image.handle)->info.view);
            }

            vk::FramebufferCreateInfo info;
            info.renderPass      = inPass;
            info.attachmentCount = static_cast<std::uint32_t>(views.size());
            info.pAttachments    = views.data();
            info.width           = inCreateInfo.width;
            info.height          = inCreateInfo.height;
            info.layers          = 1;

            return m_device->backend()->logicalDevice.createFramebuffer(info);
        }

        void VulkanRHICommandList::beginPass(const RHI::PassCreateInfo& inCreateInfo)
        {
            m_passColor = inCreateInfo.bHasColor
                              ? trackedImage(static_cast<VulkanRHIImageData*>(inCreateInfo.color.image.handle))
                              : nullptr;
            m_passDepth = inCreateInfo.bHasDepth
                              ? trackedImage(static_cast<VulkanRHIImageData*>(inCreateInfo.depth.image.handle))
                              : nullptr;

            if (m_passColor)
            {
                if (inCreateInfo.color.load == RHI::LoadOp::Load)
                {
                    setLayout(m_passColor, attachmentLayout(m_passColor));
                }
            }
            if (m_passDepth)
            {
                if (inCreateInfo.depth.load == RHI::LoadOp::Load)
                {
                    setLayout(m_passDepth, attachmentLayout(m_passDepth));
                }
            }

            vk::RenderPass  pass = framebufferPass(inCreateInfo);
            vk::Framebuffer fb   = framebuffer(inCreateInfo, pass);

            std::vector<vk::ClearValue> clears;
            if (inCreateInfo.bHasColor)
            {
                vk::ClearColorValue color(
                    inCreateInfo.color.clear.x,
                    inCreateInfo.color.clear.y,
                    inCreateInfo.color.clear.z,
                    inCreateInfo.color.clear.w
                );
                clears.push_back(color);
            }
            if (inCreateInfo.bHasDepth)
            {
                clears.push_back(vk::ClearDepthStencilValue(1.0f, 0));
            }

            vk::RenderPassBeginInfo begin;
            begin.renderPass        = pass;
            begin.framebuffer       = fb;
            begin.renderArea.extent = vk::Extent2D{inCreateInfo.width, inCreateInfo.height};
            begin.clearValueCount   = static_cast<std::uint32_t>(clears.size());
            begin.pClearValues      = clears.data();
            m_commands.beginRenderPass(&begin, vk::SubpassContents::eInline);
            m_pass        = pass;
            m_framebuffer = fb;
            m_bInPass     = true;

            if (m_passColor)
            {
                m_device->rememberLayout(m_passColor, attachmentLayout(m_passColor));
            }
            if (m_passDepth)
            {
                m_device->rememberLayout(m_passDepth, attachmentLayout(m_passDepth));
            }
        }

        void VulkanRHICommandList::endPass()
        {
            if (m_bInPass)
            {
                m_commands.endRenderPass();
                m_bInPass = false;
            }
            if (m_pass || m_framebuffer)
            {
                m_retiredPasses[commandKey(m_commands)].push_back({m_pass, m_framebuffer});
                m_pass        = nullptr;
                m_framebuffer = nullptr;
            }
            if (m_passColor && m_passColor->bIsSampled && !m_passColor->bPresent)
            {
                setLayout(m_passColor, shaderReadLayout(m_passColor));
            }
            m_passColor = nullptr;
            m_passDepth = nullptr;
        }

        void VulkanRHICommandList::bindPipeline(RHI::Pipeline inPipeline)
        {
            m_pipeline = static_cast<VulkanRHIPipelineData*>(inPipeline.handle);
            m_pipeline->pipeline.bind(m_commands);
        }

        void VulkanRHICommandList::bindGroup(std::uint32_t inSet, RHI::BindGroup inGroup)
        {
            auto* group = static_cast<VulkanRHIGroupData*>(inGroup.handle);
            m_pipeline->pipeline.bind(m_commands, inSet, group->set);
        }

        void VulkanRHICommandList::bindVertexBuffer(RHI::Buffer inBuffer)
        {
            auto*                buffer     = static_cast<VulkanRHIBufferData*>(inBuffer.handle);
            const vk::Buffer     buffers[1] = {buffer->buffer.instance};
            const vk::DeviceSize offsets[1] = {0};
            m_commands.bindVertexBuffers(0, 1, buffers, offsets);
        }

        void VulkanRHICommandList::bindIndexBuffer(RHI::Buffer inBuffer)
        {
            auto* buffer = static_cast<VulkanRHIBufferData*>(inBuffer.handle);
            m_commands.bindIndexBuffer(buffer->buffer.instance, 0, vk::IndexType::eUint32);
        }

        void VulkanRHICommandList::pushConstants(const void* inData, std::uint32_t inSize)
        {
            if (!m_pipeline || !inData || inSize == 0 || !m_pipeline->pushStages)
            {
                return;
            }

            m_commands.pushConstants(m_pipeline->pipeline.layout, m_pipeline->pushStages, 0, inSize, inData);
        }

        void VulkanRHICommandList::draw(
            std::uint32_t inVertexCount,
            std::uint32_t inInstanceCount,
            std::uint32_t inFirstVertex,
            std::uint32_t inFirstInstance
        )
        {
            m_commands.draw(inVertexCount, inInstanceCount, inFirstVertex, inFirstInstance);
        }

        void VulkanRHICommandList::drawIndexed(
            std::uint32_t inIndexCount,
            std::uint32_t inInstanceCount,
            std::uint32_t inFirstIndex,
            std::int32_t  inVertexOffset,
            std::uint32_t inFirstInstance
        )
        {
            m_commands.drawIndexed(inIndexCount, inInstanceCount, inFirstIndex, inVertexOffset, inFirstInstance);
        }

        void VulkanRHICommandList::setViewport(const RHI::Viewport& inViewport)
        {
            vk::Viewport viewport;
            viewport.x        = inViewport.position.x;
            viewport.y        = inViewport.position.y;
            viewport.width    = inViewport.size.x;
            viewport.height   = inViewport.size.y;
            viewport.minDepth = inViewport.depth.x;
            viewport.maxDepth = inViewport.depth.y;
            m_commands.setViewport(0, 1, &viewport);
        }

        void VulkanRHICommandList::setScissor(const RHI::Scissor& inScissor)
        {
            vk::Rect2D scissor;
            scissor.offset.x      = inScissor.x;
            scissor.offset.y      = inScissor.y;
            scissor.extent.width  = inScissor.width;
            scissor.extent.height = inScissor.height;
            m_commands.setScissor(0, 1, &scissor);
        }

        void VulkanRHICommandList::setLineWidth(float inWidth)
        {
            m_commands.setLineWidth(inWidth);
        }

        void VulkanRHICommandList::blitColor(
            RHI::Image    inSource,
            RHI::Image    inDestination,
            std::int32_t  inX,
            std::int32_t  inY,
            std::uint32_t inWidth,
            std::uint32_t inHeight
        )
        {
            auto* source = static_cast<VulkanRHIImageData*>(inSource.handle);
            auto* dest   = static_cast<VulkanRHIImageData*>(inDestination.handle);
            if (!source || !dest)
            {
                return;
            }

            const std::int32_t srcExtentX = static_cast<std::int32_t>(
                std::max(1u, source->info.extent.width)
            );
            const std::int32_t srcExtentY = static_cast<std::int32_t>(
                std::max(1u, source->info.extent.height)
            );
            const std::int32_t dstExtentX = static_cast<std::int32_t>(
                std::max(1u, dest->info.extent.width)
            );
            const std::int32_t dstExtentY = static_cast<std::int32_t>(
                std::max(1u, dest->info.extent.height)
            );

            const std::int32_t srcX0 = std::clamp(inX, 0, srcExtentX);
            const std::int32_t srcY0 = std::clamp(inY, 0, srcExtentY);
            const std::int32_t srcX1 = std::clamp(inX + static_cast<std::int32_t>(inWidth), 0, srcExtentX);
            const std::int32_t srcY1 = std::clamp(inY + static_cast<std::int32_t>(inHeight), 0, srcExtentY);
            if (srcX1 <= srcX0 || srcY1 <= srcY0)
            {
                return;
            }

            const std::uint32_t levels = std::max(1u, dest->mips);

            setLayout(source, vk::ImageLayout::eTransferSrcOptimal);
            setLayout(dest, vk::ImageLayout::eTransferDstOptimal);

            vk::ImageBlit region;
            region.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
            region.srcSubresource.mipLevel       = 0;
            region.srcSubresource.baseArrayLayer = 0;
            region.srcSubresource.layerCount     = 1;
            region.srcOffsets[0]                 = vk::Offset3D(srcX0, srcY0, 0);
            region.srcOffsets[1]                 = vk::Offset3D(srcX1, srcY1, 1);
            region.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
            region.dstSubresource.mipLevel       = 0;
            region.dstSubresource.baseArrayLayer = 0;
            region.dstSubresource.layerCount     = 1;
            region.dstOffsets[0]                 = vk::Offset3D(0, 0, 0);
            region.dstOffsets[1]                 = vk::Offset3D(dstExtentX, dstExtentY, 1);

            m_commands.blitImage(
                source->info.instance,
                vk::ImageLayout::eTransferSrcOptimal,
                dest->info.instance,
                vk::ImageLayout::eTransferDstOptimal,
                region,
                vk::Filter::eNearest
            );

            auto transition = [&](std::uint32_t          inLevel,
                                  vk::ImageLayout        inOldLayout,
                                  vk::ImageLayout        inNewLayout,
                                  vk::AccessFlags        inSourceAccess,
                                  vk::AccessFlags        inDestinationAccess,
                                  vk::PipelineStageFlags inSourceStage,
                                  vk::PipelineStageFlags inDestinationStage)
            {
                vk::ImageMemoryBarrier barrier;
                barrier.oldLayout                       = inOldLayout;
                barrier.newLayout                       = inNewLayout;
                barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                barrier.image                           = dest->info.instance;
                barrier.srcAccessMask                   = inSourceAccess;
                barrier.dstAccessMask                   = inDestinationAccess;
                barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
                barrier.subresourceRange.baseMipLevel   = inLevel;
                barrier.subresourceRange.levelCount     = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount     = 1;
                m_commands.pipelineBarrier(
                    inSourceStage,
                    inDestinationStage,
                    vk::DependencyFlags(),
                    nullptr,
                    nullptr,
                    barrier
                );
            };

            std::uint32_t mipWidth  = static_cast<std::uint32_t>(dstExtentX);
            std::uint32_t mipHeight = static_cast<std::uint32_t>(dstExtentY);
            for (std::uint32_t level = 1; level < levels; level++)
            {
                transition(
                    level - 1,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eTransferSrcOptimal,
                    vk::AccessFlagBits::eTransferWrite,
                    vk::AccessFlagBits::eTransferRead,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eTransfer
                );

                const std::uint32_t nextWidth  = std::max(1u, mipWidth / 2);
                const std::uint32_t nextHeight = std::max(1u, mipHeight / 2);

                vk::ImageBlit blit;
                blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
                blit.srcSubresource.mipLevel       = level - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount     = 1;
                blit.srcOffsets[0]                 = vk::Offset3D(0, 0, 0);
                blit.srcOffsets[1] =
                    vk::Offset3D(static_cast<std::int32_t>(mipWidth), static_cast<std::int32_t>(mipHeight), 1);
                blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
                blit.dstSubresource.mipLevel       = level;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount     = 1;
                blit.dstOffsets[0]                 = vk::Offset3D(0, 0, 0);
                blit.dstOffsets[1] =
                    vk::Offset3D(static_cast<std::int32_t>(nextWidth), static_cast<std::int32_t>(nextHeight), 1);

                m_commands.blitImage(
                    dest->info.instance,
                    vk::ImageLayout::eTransferSrcOptimal,
                    dest->info.instance,
                    vk::ImageLayout::eTransferDstOptimal,
                    blit,
                    vk::Filter::eLinear
                );

                transition(
                    level - 1,
                    vk::ImageLayout::eTransferSrcOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::AccessFlagBits::eTransferRead,
                    vk::AccessFlagBits::eShaderRead,
                    vk::PipelineStageFlagBits::eTransfer,
                    vk::PipelineStageFlagBits::eFragmentShader
                );

                mipWidth  = nextWidth;
                mipHeight = nextHeight;
            }

            transition(
                levels - 1,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::AccessFlagBits::eTransferWrite,
                vk::AccessFlagBits::eShaderRead,
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader
            );

            m_device->rememberLayout(dest, vk::ImageLayout::eShaderReadOnlyOptimal);
            setLayout(source, vk::ImageLayout::eColorAttachmentOptimal);
        }

        void VulkanRHICommandList::prepareShaderRead(RHI::Image inImage)
        {
            auto* image = static_cast<VulkanRHIImageData*>(inImage.handle);
            if (!image)
            {
                return;
            }

            setLayout(image, shaderReadLayout(image));
        }

        void VulkanRHICommandList::preparePresent(RHI::Image inImage)
        {
            auto* image = static_cast<VulkanRHIImageData*>(inImage.handle);
            if (!image)
            {
                return;
            }

            setLayout(image, vk::ImageLayout::ePresentSrcKHR);
        }
    }
}
