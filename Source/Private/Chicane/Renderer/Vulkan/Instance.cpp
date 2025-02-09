#include "Chicane/Renderer/Vulkan/Instance.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Instance
        {
            bool areExtensionsSupported(const std::vector<const char*>& inExtensions)
            {
                bool bIsSupported = false;

                auto supportedExtensions = vk::enumerateInstanceExtensionProperties();

                for (const char* extensionName : inExtensions)
                {
                    bIsSupported = false;

                    for (const auto& extensionProperties : supportedExtensions)
                    {
                        if (!String::areEquals(extensionName, extensionProperties.extensionName))
                        {
                            bIsSupported = true;

                            break;
                        }
                    }

                    if (!bIsSupported)
                    {
                        return false;
                    }
                }

                return true;
            }

            bool areValidationLayersSupported(const std::vector<const char*>& inValidationLayers)
            {
                bool bIsSupported = false;

                auto supportedLayers = vk::enumerateInstanceLayerProperties();

                for (const char* layerName : inValidationLayers)
                {
                    bIsSupported = false;

                    for (const auto& supportedLayer : supportedLayers)
                    {
                        if (strcmp(layerName, supportedLayer.layerName) == 0)
                        {
                            bIsSupported = true;

                            break;
                        }
                    }

                    if (!bIsSupported)
                    {
                        return false;
                    }
                }

                return true;
            }
        
            void init(
                vk::Instance& outInstance,
                vk::DispatchLoaderDynamic& outDldi,
                SDL_Window* inWindow
            )
            {
                std::uint32_t sdlExtensionCount = 0;

                const char* const* sdlRawExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

                if (!sdlRawExtensions)
                {
                    throw std::runtime_error(SDL_GetError());
                }

                std::vector<const char*> extensions(
                    sdlRawExtensions,
                    sdlRawExtensions + sdlExtensionCount
                );

                if (IS_DEBUGGING)
                {
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                }

                if (!areExtensionsSupported(extensions))
                {
                    throw std::runtime_error("SDL layers are not fully supported");
                }

                std::vector<const char*> layers = { LAYERS.begin(), LAYERS.end() };

                if (!areValidationLayersSupported(layers))
                {
                    throw std::runtime_error("Validation Layers are not fully supported");
                }

                vk::ApplicationInfo applicationInfo = {};
                applicationInfo.pApplicationName    = APPLICATION_NAME;
                applicationInfo.applicationVersion  = VK_MAKE_API_VERSION(0, 0, 0, 1);
                applicationInfo.pEngineName         = ENGINE_NAME;
                applicationInfo.engineVersion       = VK_MAKE_API_VERSION(0, 0, 0, 1);
                applicationInfo.apiVersion          = VK_MAKE_API_VERSION(0, 1, 3, 0);

                vk::InstanceCreateInfo createInfo  = {};
                createInfo.pApplicationInfo        = &applicationInfo;
                createInfo.enabledLayerCount       = static_cast<std::uint32_t>(layers.size());
                createInfo.ppEnabledLayerNames     = layers.data();
                createInfo.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
                createInfo.ppEnabledExtensionNames = extensions.data();

                outInstance = vk::createInstance(createInfo);
                outDldi     = vk::DispatchLoaderDynamic(outInstance, vkGetInstanceProcAddr);
            }
        }
    }
}