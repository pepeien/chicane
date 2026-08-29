#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"

#include <array>

#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sync.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void VulkanFrame::wait()
        {
            vk::Result result = logicalDevice.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
            if (result != vk::Result::eSuccess && result != vk::Result::eTimeout)
            {
                throw std::runtime_error("Error while waiting the fences");
            }
        }

        void VulkanFrame::reset()
        {
            vk::Result result = logicalDevice.resetFences(1, &fence);
            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while resetting the fences");
            }
        }

        void VulkanFrame::begin(const Frame& inFrame, const VulkanSwapchainImage& inImage)
        {
            commandBuffer.reset();

            updateCameraData(inFrame.getCamera());
            updateLightData(inFrame.getLights());
            update2DData(inFrame.getInstances2D());
            update3DData(inFrame.getInstances3D());

            image = inImage;

            vk::CommandBufferBeginInfo commandBufferBegin;
            commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            commandBuffer.begin(commandBufferBegin);

            vk::ImageMemoryBarrier targetToColor;
            targetToColor.oldLayout           = vk::ImageLayout::eUndefined;
            targetToColor.newLayout           = vk::ImageLayout::eColorAttachmentOptimal;
            targetToColor.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            targetToColor.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            targetToColor.image               = image.targetImage.instance;
            targetToColor.srcAccessMask       = vk::AccessFlagBits::eNone;
            targetToColor.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
            targetToColor.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            targetToColor.subresourceRange.baseMipLevel   = 0;
            targetToColor.subresourceRange.levelCount     = 1;
            targetToColor.subresourceRange.baseArrayLayer = 0;
            targetToColor.subresourceRange.layerCount     = 1;

            vk::ImageMemoryBarrier depthToAttachment;
            depthToAttachment.oldLayout           = vk::ImageLayout::eUndefined;
            depthToAttachment.newLayout           = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthToAttachment.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthToAttachment.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthToAttachment.image               = image.depthImage.instance;
            depthToAttachment.srcAccessMask       = vk::AccessFlagBits::eNone;
            depthToAttachment.dstAccessMask =
                vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            depthToAttachment.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eDepth;
            depthToAttachment.subresourceRange.baseMipLevel   = 0;
            depthToAttachment.subresourceRange.levelCount     = 1;
            depthToAttachment.subresourceRange.baseArrayLayer = 0;
            depthToAttachment.subresourceRange.layerCount     = 1;

            std::array<vk::ImageMemoryBarrier, 2> barriers = {targetToColor, depthToAttachment};
            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests |
                    vk::PipelineStageFlagBits::eLateFragmentTests,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                barriers
            );
        }

        void VulkanFrame::flushTarget()
        {
            vk::ImageSubresourceRange range;
            range.aspectMask     = vk::ImageAspectFlagBits::eColor;
            range.baseMipLevel   = 0;
            range.levelCount     = 1;
            range.baseArrayLayer = 0;
            range.layerCount     = 1;

            vk::ImageMemoryBarrier targetToTransfer;
            targetToTransfer.oldLayout           = vk::ImageLayout::eColorAttachmentOptimal;
            targetToTransfer.newLayout           = vk::ImageLayout::eTransferSrcOptimal;
            targetToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            targetToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            targetToTransfer.image               = image.targetImage.instance;
            targetToTransfer.srcAccessMask       = vk::AccessFlagBits::eColorAttachmentWrite;
            targetToTransfer.dstAccessMask       = vk::AccessFlagBits::eTransferRead;
            targetToTransfer.subresourceRange    = range;

            vk::ImageMemoryBarrier swapchainToTransfer;
            swapchainToTransfer.oldLayout           = vk::ImageLayout::eUndefined;
            swapchainToTransfer.newLayout           = vk::ImageLayout::eTransferDstOptimal;
            swapchainToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            swapchainToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            swapchainToTransfer.image               = image.colorImage.instance;
            swapchainToTransfer.srcAccessMask       = vk::AccessFlagBits::eNone;
            swapchainToTransfer.dstAccessMask       = vk::AccessFlagBits::eTransferWrite;
            swapchainToTransfer.subresourceRange    = range;

            std::array<vk::ImageMemoryBarrier, 2> before = {targetToTransfer, swapchainToTransfer};
            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                before
            );

            vk::ImageCopy region;
            region.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
            region.srcSubresource.mipLevel       = 0;
            region.srcSubresource.baseArrayLayer = 0;
            region.srcSubresource.layerCount     = 1;
            region.dstSubresource                = region.srcSubresource;
            region.extent = vk::Extent3D(image.targetImage.extent.width, image.targetImage.extent.height, 1);

            commandBuffer.copyImage(
                image.targetImage.instance,
                vk::ImageLayout::eTransferSrcOptimal,
                image.colorImage.instance,
                vk::ImageLayout::eTransferDstOptimal,
                region
            );

            vk::ImageMemoryBarrier swapchainToColor = swapchainToTransfer;
            swapchainToColor.oldLayout              = vk::ImageLayout::eTransferDstOptimal;
            swapchainToColor.newLayout              = vk::ImageLayout::eColorAttachmentOptimal;
            swapchainToColor.srcAccessMask          = vk::AccessFlagBits::eTransferWrite;
            swapchainToColor.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

            vk::ImageMemoryBarrier targetToSample = targetToTransfer;
            targetToSample.oldLayout              = vk::ImageLayout::eTransferSrcOptimal;
            targetToSample.newLayout              = vk::ImageLayout::eShaderReadOnlyOptimal;
            targetToSample.srcAccessMask          = vk::AccessFlagBits::eTransferRead;
            targetToSample.dstAccessMask          = vk::AccessFlagBits::eShaderRead;

            std::array<vk::ImageMemoryBarrier, 2> after = {swapchainToColor, targetToSample};
            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eFragmentShader,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                after
            );
        }

        void VulkanFrame::end()
        {
            vk::ImageMemoryBarrier presentBarrier;
            presentBarrier.oldLayout                       = vk::ImageLayout::eColorAttachmentOptimal;
            presentBarrier.newLayout                       = vk::ImageLayout::ePresentSrcKHR;
            presentBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            presentBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            presentBarrier.image                           = image.colorImage.instance;
            presentBarrier.srcAccessMask                   = vk::AccessFlagBits::eColorAttachmentWrite;
            presentBarrier.dstAccessMask                   = vk::AccessFlagBits::eNone;
            presentBarrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            presentBarrier.subresourceRange.baseMipLevel   = 0;
            presentBarrier.subresourceRange.levelCount     = 1;
            presentBarrier.subresourceRange.baseArrayLayer = 0;
            presentBarrier.subresourceRange.layerCount     = 1;

            commandBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::PipelineStageFlagBits::eBottomOfPipe,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                presentBarrier
            );

            commandBuffer.end();
        }

        void VulkanFrame::destroy()
        {
            destroyCameraData();
            destroyLightData();
            destroy2DData();
            destroy3DData();
            destroySync();
        }

        void VulkanFrame::setupCommandBuffer(const vk::CommandPool& inPool)
        {
            VulkanCommandBuffer::init(commandBuffer, {logicalDevice, inPool});
        }

        void VulkanFrame::setupSync()
        {
            VulkanSync::initSempahore(imageAvailableSemaphore, logicalDevice);
            VulkanSync::initFence(fence, logicalDevice);
        }

        void VulkanFrame::destroySync()
        {
            logicalDevice.destroyFence(fence);
            logicalDevice.destroySemaphore(imageAvailableSemaphore);
        }

        void VulkanFrame::setupCameraData()
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = sizeof(View);
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;

            cameraResource.setup(bufferCreateInfo);
        }

        void VulkanFrame::updateCameraData(const View& inData)
        {
            View data = inData;
            data.flipY();

            cameraResource.copyToBuffer(&data, sizeof(View));
        }

        void VulkanFrame::destroyCameraData()
        {
            cameraResource.destroy(logicalDevice);
        }

        void VulkanFrame::setupLightData()
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = sizeof(View);
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;

            lightResource.setup(bufferCreateInfo);
        }

        void VulkanFrame::updateLightData(const View::List& inData)
        {
            if (inData.empty())
            {
                return;
            }

            View data = inData.at(0);
            data.flipY();

            lightResource.copyToBuffer(&data, sizeof(View));
        }

        void VulkanFrame::destroyLightData()
        {
            lightResource.destroy(logicalDevice);
        }

        void VulkanFrame::setup2DData(std::size_t inBudget)
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = inBudget;
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;

            poly2DResource.setup(bufferCreateInfo);
        }

        void VulkanFrame::update2DData(const DrawPoly2DInstance::List& inData)
        {
            if (inData.empty())
            {
                return;
            }

            poly2DResource.copyToBuffer(inData.data(), sizeof(DrawPoly2DInstance) * inData.size());
        }

        void VulkanFrame::destroy2DData()
        {
            poly2DResource.destroy(logicalDevice);
        }

        void VulkanFrame::setup3DData(std::size_t inBudget)
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = inBudget;
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;

            poly3DResource.setup(bufferCreateInfo);
        }

        void VulkanFrame::update3DData(const DrawPoly3DInstance::List& inData)
        {
            if (inData.empty())
            {
                return;
            }

            poly3DResource.copyToBuffer(inData.data(), sizeof(DrawPoly3DInstance) * inData.size());
        }

        void VulkanFrame::destroy3DData()
        {
            poly3DResource.destroy(logicalDevice);
        }

        void VulkanFrame::addDescriptorSet(const String& inId, const vk::DescriptorSet& inDescriptorSet)
        {
            if (descriptorSets.find(inId) != descriptorSets.end())
            {
                throw std::runtime_error("Descriptor set " + inId + " already exists");
            }

            descriptorSets.insert(std::make_pair(inId, inDescriptorSet));
        }

        vk::DescriptorSet VulkanFrame::getDescriptorSet(const String& inId) const
        {
            return descriptorSets.at(inId);
        }

        void VulkanFrame::addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet)
        {
            descriptorSetWrites.push_back(inWriteDescriptorSet);

            updateDescriptorSets();
        }

        void VulkanFrame::updateDescriptorSets()
        {
            if (descriptorSetWrites.empty())
            {
                return;
            }

            logicalDevice.updateDescriptorSets(descriptorSetWrites, nullptr);
        }
    }
}