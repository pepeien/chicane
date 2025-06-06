#include "Chicane/Runtime/Vulkan/Instance.hpp"

#include <SDL3/SDL_vulkan.h>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Instance
        {
            bool areExtensionsSupported(const std::vector<const char*>& inExtensions)
            {
                std::set<std::string> extensions(inExtensions.begin(), inExtensions.end());

                for (vk::ExtensionProperties& extension : vk::enumerateInstanceExtensionProperties())
                {
                    extensions.erase(extension.extensionName);
                }

                for (const std::string& unsupportedExtension : extensions)
                {
                    Log::warning(
                        "The vulkan instance extension [%s] is not supported",
                        unsupportedExtension.c_str()
                    );
                }

                return extensions.empty();
            }

            bool areLayersSupported(const std::vector<const char*>& inLayers)
            {
                std::set<std::string> layers(inLayers.begin(), inLayers.end());

                for (vk::LayerProperties& layer : vk::enumerateInstanceLayerProperties())
                {
                    layers.erase(layer.layerName);
                }

                for (const std::string& unsupportedLayer : layers)
                {
                    Log::warning(
                        "The vulkan instance layer [%s] is not supported",
                        unsupportedLayer.c_str()
                    );
                }

                return layers.empty();
            }
        
            void init(vk::Instance& outInstance, vk::DispatchLoaderDynamic& outDldi)
            {
                std::uint32_t sdlExtensionCount = 0;

                const char* const* sdlRawExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

                if (!sdlRawExtensions)
                {
                    throw std::runtime_error(SDL_GetError());
                }

                std::vector<const char*> extensions = EXTENSIONS;
                extensions.insert(
                    extensions.end(),
                    sdlRawExtensions,
                    sdlRawExtensions + sdlExtensionCount
                );

                if (!areExtensionsSupported(extensions))
                {
                    throw std::runtime_error("Extensions are not fully supported");
                }

                std::vector<const char*> layers = LAYERS;

                if (!areLayersSupported(layers))
                {
                    throw std::runtime_error("Layers are not fully supported");
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