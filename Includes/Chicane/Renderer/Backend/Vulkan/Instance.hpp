#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanInstance
        {
            static const std::vector<const char*> LAYERS = {
#if IS_DEBUGGING
                "VK_LAYER_KHRONOS_validation"
#endif
            };

            static const std::vector<const char*> EXTENSIONS = {
#if IS_DEBUGGING
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
            };

            CHICANE_RENDERER bool areExtensionsSupported(const std::vector<const char*>& inExtensions);
            CHICANE_RENDERER bool areLayersSupported(const std::vector<const char*>& inLayers);

            CHICANE_RENDERER void init(vk::Instance& outInstance, vk::detail::DispatchLoaderDynamic& outDldi);
        }
    }
}