#include "Instance.hpp"

static bool areExtensionsSupported(std::vector<const char*> inExtensions)
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

static bool areValidationLayersSupported(std::vector<const char*> inValidationLayers)
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
		
namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Instance
			{
				void init(vk::Instance& outInstance, vk::DispatchLoaderDynamic& outDldi)
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

					uint32_t version { 0 };
					vkEnumerateInstanceVersion(&version);

					// Drop Vulkan required version by one MAJOR to ensure compability
					version = VK_MAKE_API_VERSION(0, 1, 0, 0);

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

					outInstance = vk::createInstance(createInfo);
					outDldi     = vk::DispatchLoaderDynamic(outInstance, vkGetInstanceProcAddr);
				}
			}
		}
	}
}