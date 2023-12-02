#include "Device.h"
#include "Core/Queue.h"

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
	namespace Core
	{
		namespace Mounter
		{
			namespace Vulkan
			{
				void pickPhysicalDevice(vk::PhysicalDevice& allocator, vk::Instance& inInstance)
				{
					for (vk::PhysicalDevice physicalDevice : inInstance.enumeratePhysicalDevices())
					{
						if (isPhysicalDeviceSuitable(physicalDevice))
						{
							allocator = physicalDevice;

							return;
						}
					}

					throw std::runtime_error("Failed to pick a suitable phyisical device");
				}

				void initLogicalDevice(vk::Device& allocator, vk::PhysicalDevice& inPhysicalDevice)
				{
            		Core::Vulkan::Queue::FamilyIndices familyIndices;
					Core::Vulkan::Queue::findFamilyInidices(familyIndices, inPhysicalDevice);

					float queuePriority = 1.0f;

					vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo(
						vk::DeviceQueueCreateFlags(),
						familyIndices.graphicsFamily.value(),
						1,
						&queuePriority
					);

					vk::PhysicalDeviceFeatures logicalDeviceFeatures = vk::PhysicalDeviceFeatures();

					std::vector<const char*> usedLayers;
					
					if (IS_DEBUGGING)
					{
						usedLayers.insert(usedLayers.end(), VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
					}

					vk::DeviceCreateInfo logicalDeviceInfo = vk::DeviceCreateInfo(
						vk::DeviceCreateFlags(),
						1,
						&queueCreateInfo,
						static_cast<uint32_t>(usedLayers.size()),
						usedLayers.data(),
						0,
						nullptr,
						&logicalDeviceFeatures
					);

					allocator = inPhysicalDevice.createDevice(logicalDeviceInfo);
				}
			}
		}
	}
}