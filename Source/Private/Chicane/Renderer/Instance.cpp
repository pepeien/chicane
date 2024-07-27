#include "Chicane/Renderer/Instance.hpp"
        
namespace Chicane
{
    namespace Instance
    {
        bool areExtensionsSupported(const std::vector<const char*>& inExtensions)
        {
            bool isSupported = false;

            auto supportedExtensions = vk::enumerateInstanceExtensionProperties();

            for (const char* extensionName : inExtensions)
            {
                isSupported = false;

                for (const auto& extensionProperties : supportedExtensions)
                {
                    if (strcmp(extensionName, extensionProperties.extensionName) != 0)
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

        bool areValidationLayersSupported(const std::vector<const char*>& inValidationLayers)
        {
            bool isSupported = false;

            auto supportedLayers = vk::enumerateInstanceLayerProperties();

            for (const char* layerName : inValidationLayers)
            {
                isSupported = false;

                for (const auto& supportedLayer : supportedLayers)
                {
                    if (strcmp(layerName, supportedLayer.layerName) == 0)
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
    
        void init(
            vk::Instance& outInstance,
            vk::DispatchLoaderDynamic& outDldi,
            SDL_Window* inWindow
        )
        {
            uint32_t sdlExtensionCount = 0;

            if (
                SDL_Vulkan_GetInstanceExtensions(
                    inWindow,
                    &sdlExtensionCount,
                    nullptr
                ) == SDL_FALSE
            )
            {
                throw std::runtime_error(SDL_GetError());
            }

            const char** sdlRawExtensions = new const char*[sdlExtensionCount];

            if (
                SDL_Vulkan_GetInstanceExtensions(
                    inWindow,
                    &sdlExtensionCount,
                    sdlRawExtensions
                ) == SDL_FALSE
            )
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

            if (areExtensionsSupported(extensions) == false)
            {
                throw std::runtime_error("SDL layers are not fully supported");
            }

            if (areValidationLayersSupported(LAYERS) == false)
            {
                throw std::runtime_error("Validation Layers are not fully supported");
            }

            uint32_t version { 0 };
            vkEnumerateInstanceVersion(&version);

            // Picked MAJOR version to ensure compability
            version = VK_MAKE_API_VERSION(0, 1, 3, 0);

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

                static_cast<uint32_t>(LAYERS.size()),
                LAYERS.data(),

                static_cast<uint32_t>(extensions.size()),
                extensions.data()
            );

            outInstance = vk::createInstance(createInfo);
            outDldi     = vk::DispatchLoaderDynamic(outInstance, vkGetInstanceProcAddr);
        }
    }
}