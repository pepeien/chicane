#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"

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

            result = logicalDevice.resetFences(1, &fence);
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
        }

        void VulkanFrame::end()
        {
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