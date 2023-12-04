#include "Device.h"

#include "Core/Vulkan/Queue.h"

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
		namespace Vulkan
		{
			namespace Mounter
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

				void initLogicalDevice(
					vk::Device& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
            		Vulkan::Queue::FamilyIndices familyIndices;
					Vulkan::Queue::findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

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

					allocator = inPhysicalDevice.createDevice(logicalDeviceInfo);
				}
			}
		}
	}
}