#include "Chicane/Renderer/Backend/Vulkan/Device.hpp"

#include <set>

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer/Backend/Vulkan/Queue.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDevice
        {
            bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& inDevice)
            {
                std::set<String> extensions(EXTENSIONS.begin(), EXTENSIONS.end());

                for (vk::ExtensionProperties& extension : inDevice.enumerateDeviceExtensionProperties())
                {
                    extensions.erase(extension.extensionName.data());
                }

                for (const String& unsupportedExtension : extensions)
                {
                    Log::warning("The vulkan device extension [%s] is not supported", unsupportedExtension.toChar());
                }

                return extensions.empty();
            }

            void pickPhysicalDevice(vk::PhysicalDevice& outDevice, const vk::Instance& inInstance)
            {
                for (const vk::PhysicalDevice& device : inInstance.enumeratePhysicalDevices())
                {
                    if (!isPhysicalDeviceSuitable(device))
                    {
                        continue;
                    }

                    outDevice = device;

                    return;
                }

                throw std::runtime_error("Failed to pick a suitable physical device");
            }

            void initLogicalDevice(
                vk::Device& outDevice, const vk::PhysicalDevice& inPhysicalDevice, const vk::SurfaceKHR& inSurface
            )
            {
                VulkanQueueFamilyIndices familyIndices(inPhysicalDevice, inSurface);

                std::vector<std::uint32_t> uniqueIndices;
                uniqueIndices.push_back(familyIndices.graphicsFamily.value());

                if (familyIndices.graphicsFamily.value() != familyIndices.presentFamily.value())
                {
                    uniqueIndices.push_back(familyIndices.presentFamily.value());
                }

                float queuePriority = 1.0f;

                std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};

                for (std::uint32_t uniqueIndice : uniqueIndices)
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

                vk::PhysicalDeviceFeatures features = vk::PhysicalDeviceFeatures();
                features.fillModeNonSolid           = true;
                features.depthClamp                 = true;

                vk::PhysicalDeviceRobustness2FeaturesKHR features2;
                features2.nullDescriptor = true;

                vk::PhysicalDeviceDescriptorIndexingFeatures descriptorFeatures;
                descriptorFeatures.runtimeDescriptorArray = true;
                descriptorFeatures.pNext                  = &features2;

                vk::DeviceCreateInfo logicalDeviceInfo = vk::DeviceCreateInfo(
                    vk::DeviceCreateFlags(),

                    static_cast<std::uint32_t>(queueCreateInfos.size()),
                    queueCreateInfos.data(),

                    static_cast<std::uint32_t>(LAYERS.size()),
                    LAYERS.data(),

                    static_cast<std::uint32_t>(EXTENSIONS.size()),
                    EXTENSIONS.data(),

                    &features
                );
                logicalDeviceInfo.pNext = &descriptorFeatures;

                outDevice = inPhysicalDevice.createDevice(logicalDeviceInfo);
            }

            std::uint32_t findMemoryTypeIndex(
                const vk::PhysicalDevice& inPhysicalDevice,
                std::uint32_t             inSupportedMemoryIndices,
                vk::MemoryPropertyFlags   inRequestMemoryProperties
            )
            {
                vk::PhysicalDeviceMemoryProperties memoryProperties = inPhysicalDevice.getMemoryProperties();

                for (std::uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
                {
                    bool bIsSupported  = static_cast<bool>(inSupportedMemoryIndices & (1 << i));
                    bool bIsSufficient = static_cast<bool>(
                        (memoryProperties.memoryTypes[i].propertyFlags & inRequestMemoryProperties) ==
                        inRequestMemoryProperties
                    );

                    if (bIsSupported && bIsSufficient)
                    {
                        return i;
                    }
                }

                return 0;
            }
        }
    }
}