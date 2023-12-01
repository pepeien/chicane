#include "Mounter.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	Engine::Core::Log log;

	log.error(pCallbackData->pMessage);

	return VK_FALSE;
}

namespace Engine
{
	namespace Core
	{
		// Vulkan
		void Mounter::initVulkanDebugMessenger(
			vk::DebugUtilsMessengerEXT& inDebugMessenger,
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

			inDebugMessenger = inInstance.createDebugUtilsMessengerEXT(createInfo, nullptr, inDldi);
		}

		void Mounter::initVulkanInstance(vk::Instance& inInstance, vk::DispatchLoaderDynamic& inDldi)
		{
			uint32_t glfwExtensionCount             = 0;
			const char** glfwRequiredExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			std::vector<const char*> glfwExtensions = std::vector(glfwRequiredExtensions, glfwRequiredExtensions + glfwExtensionCount);

			if (IS_DEBUGGING)
			{
				glfwExtensions.push_back("VK_EXT_debug_utils");
			}

			if (areExtensionsSupported(glfwExtensions) == false)
			{
				throw std::runtime_error("GLFW extensions are not fully supported");
			}

			if (areValidationLayersSupported(VALIDATION_LAYERS) == false)
			{
				throw std::runtime_error("Validation Layers are not fully supported");
			}

			// Drop Vulkan required version by one MAJOR to ensure compability
			uint32_t version = VK_MAKE_API_VERSION(0, 1, 0, 0);

			vk::ApplicationInfo applicationInfo = vk::ApplicationInfo(
				APPLICATION_NAME,
				version,
				ENGINE_NAME,
				version,
				version
			);

			vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
				vk::InstanceCreateFlags(),
				&applicationInfo,
				static_cast<uint32_t>(VALIDATION_LAYERS.size()),
				VALIDATION_LAYERS.data(),
				static_cast<uint32_t>(glfwExtensions.size()),
				glfwExtensions.data()
			);

			inInstance = vk::createInstance(createInfo, nullptr);
			inDldi     = vk::DispatchLoaderDynamic(inInstance, vkGetInstanceProcAddr);
		}

		// GLFW
		GLFWwindow* Mounter::initGlfwWindow()
		{
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

            return glfwCreateWindow(
                desktop->width,
                desktop->height,
                APPLICATION_NAME,
                nullptr,
                nullptr
            );
		}

		// Vulkan
		bool Mounter::areExtensionsSupported(std::vector<const char*> inExtensions)
        {
			bool isSupported;
            for (const char* extensionName : inExtensions)
            {
				isSupported = false;

                for (const auto& extensionProperties : vk::enumerateInstanceExtensionProperties())
                {
                    if (strcmp(extensionName, extensionProperties.extensionName) == 0)
                    {
                       	isSupported = true;

						break;
                    }
                }

				if (isSupported == false)
				{
					return false;
				}
            }

			return true;
        }

		bool Mounter::areValidationLayersSupported(std::vector<const char*> inValidationLayers)
		{
			bool isSupported;
            for (const char* layerName : inValidationLayers)
            {
				isSupported = false;

                for (const auto& layerProperties : vk::enumerateInstanceLayerProperties())
                {
                    if (strcmp(layerName, layerProperties.layerName) == 0)
                    {
                        isSupported = true;

						break;
                    }
                }

				if (isSupported == false)
				{
					return false;
				}
            }

			return true;
		}
	}
}