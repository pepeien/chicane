#include "Debug.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	switch (messageType)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		Engine::Core::Log::info(pCallbackData->pMessage);

		break;

	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		Engine::Core::Log::warning(pCallbackData->pMessage);

		break;

	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		Engine::Core::Log::error(pCallbackData->pMessage);

		break;

	default:
		Engine::Core::Log::critical(pCallbackData->pMessage);

		break;
	}

	return VK_FALSE;
}

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace Mounter
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