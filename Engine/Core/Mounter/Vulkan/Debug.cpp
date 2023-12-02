#include "Debug.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
	{
		Engine::Core::Log::info(pCallbackData->pMessage);

		return VK_FALSE;
	}

	if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
	{
		Engine::Core::Log::warning(pCallbackData->pMessage);

		return VK_FALSE;
	}

	if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
	{
		Engine::Core::Log::error(pCallbackData->pMessage);

		return VK_FALSE;
	}

	Engine::Core::Log::critical(pCallbackData->pMessage);

	return VK_FALSE;
}

namespace Engine
{
	namespace Core
	{
		namespace Mounter
		{
			namespace Vulkan
			{
				void initDebugMessenger(
					vk::DebugUtilsMessengerEXT& allocator,
					vk::Instance& inInstance,
					vk::DispatchLoaderDynamic& inDldi
				)
				{
					if (IS_DEBUGGING == false)
					{
						return;
					}
	
					vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
						vk::DebugUtilsMessengerCreateFlagsEXT(),

						// Severity
						vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
						vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,

						// Type
						vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
						vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,

						debugCallback,
						nullptr
					);

					allocator = inInstance.createDebugUtilsMessengerEXT(createInfo, nullptr, inDldi);
				}
			}
		}
	}
}