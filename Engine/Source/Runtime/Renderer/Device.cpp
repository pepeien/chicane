#include "Device.hpp"

static bool isPhysicalDeviceSuitable(vk::PhysicalDevice& inPhysicalDevice)
{
    std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

    for (vk::ExtensionProperties& extension : inPhysicalDevice.enumerateDeviceExtensionProperties())
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Device
            {
                void pickPhysicalDevice(vk::PhysicalDevice& outPhysicalDevice, vk::Instance& inInstance)
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
                    vk::Device& outPhysicalDevice,
                    vk::PhysicalDevice& inPhysicalDevice,
                    vk::SurfaceKHR& inSurface
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

                    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
                    
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

                    std::vector<const char*> usedLayers;
                    
                    if (IS_DEBUGGING)
                    {
                        usedLayers.insert(usedLayers.end(), VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
                    }

                    vk::PhysicalDeviceFeatures logicalDeviceFeatures = vk::PhysicalDeviceFeatures();

                    vk::DeviceCreateInfo logicalDeviceInfo = vk::DeviceCreateInfo(
                        vk::DeviceCreateFlags(),

                        static_cast<uint32_t>(queueCreateInfos.size()),
                        queueCreateInfos.data(),

                        static_cast<uint32_t>(usedLayers.size()),
                        usedLayers.data(),

                        static_cast<uint32_t>(DEVICE_EXTENSIONS.size()),
                        DEVICE_EXTENSIONS.data(),

                        &logicalDeviceFeatures
                    );

                    outPhysicalDevice = inPhysicalDevice.createDevice(logicalDeviceInfo);
                }

                uint32_t findMemoryTypeIndex(
                    vk::PhysicalDevice& inPhysicalDevice,
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
    }
}