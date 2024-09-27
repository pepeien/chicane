#include "Chicane/Renderer/Device.hpp"

namespace Chicane
{
    namespace Device
    {
        bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inPhysicalDevice)
        {
            std::set<std::string> requiredExtensions(EXTENSIONS.begin(), EXTENSIONS.end());

            for (vk::ExtensionProperties& extension : inPhysicalDevice.enumerateDeviceExtensionProperties())
            {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }

        void pickPhysicalDevice(vk::PhysicalDevice& outPhysicalDevice, const vk::Instance& inInstance)
        {
            for (vk::PhysicalDevice physicalDevice : inInstance.enumeratePhysicalDevices())
            {
                if (isPhysicalDeviceSuitable(physicalDevice))
                {
                    outPhysicalDevice = physicalDevice;

                    return;
                }
            }

            throw std::runtime_error("Failed to pick a suitable physical device");
        }

        void initLogicalDevice(
            vk::Device& outLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::SurfaceKHR& inSurface
        )
        {
            Queue::FamilyIndices familyIndices;
            Queue::findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

            std::vector<uint32_t> uniqueIndices;
            uniqueIndices.push_back(familyIndices.graphicsFamily.value());

            if (familyIndices.graphicsFamily.value() != familyIndices.presentFamily.value())
            {
                uniqueIndices.push_back(familyIndices.presentFamily.value());
            }

            float queuePriority = 1.0f;

            std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos {};
            
            for (uint32_t uniqueIndice : uniqueIndices)
            {
                queueCreateInfos.push_back(
                    vk::DeviceQueueCreateInfo(
                        vk::DeviceQueueCreateFlags(),
                        familyIndices.graphicsFamily.value(),
                        1,
                        &queuePriority
                    )
                );
            }

            std::vector<const char*> layers     = LAYERS;
            std::vector<const char*> extensions = EXTENSIONS;

            vk::PhysicalDeviceFeatures features = vk::PhysicalDeviceFeatures();
            features.fillModeNonSolid = true;

            vk::PhysicalDeviceDescriptorIndexingFeatures descriptorFeatures = vk::PhysicalDeviceDescriptorIndexingFeatures();
            descriptorFeatures.runtimeDescriptorArray = true;

            vk::DeviceCreateInfo logicalDeviceInfo = vk::DeviceCreateInfo(
                vk::DeviceCreateFlags(),

                static_cast<uint32_t>(queueCreateInfos.size()),
                queueCreateInfos.data(),

                static_cast<uint32_t>(layers.size()),
                layers.data(),

                static_cast<uint32_t>(extensions.size()),
                extensions.data(),

                &features
            );
            logicalDeviceInfo.pNext = &descriptorFeatures;

            outLogicalDevice = inPhysicalDevice.createDevice(logicalDeviceInfo);
        }

        uint32_t findMemoryTypeIndex(
            const vk::PhysicalDevice& inPhysicalDevice,
            uint32_t inSupportedMemoryIndices,
            vk::MemoryPropertyFlags inRequestMemoryProperties
        )
        {
            vk::PhysicalDeviceMemoryProperties memoryProperties = inPhysicalDevice.getMemoryProperties();

            for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
            {
                bool isSupported{ static_cast<bool>(inSupportedMemoryIndices & (1 << i)) };
                bool isSufficient{
                    (memoryProperties.memoryTypes[i].propertyFlags & inRequestMemoryProperties) == inRequestMemoryProperties
                };

                if (isSupported && isSufficient)
                {
                    return i;
                }
            }

            return 0;
        }
    }
}