#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Instance
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

            CHICANE_RUNTIME bool areExtensionsSupported(const std::vector<const char*>& inExtensions);
            CHICANE_RUNTIME bool areLayersSupported(const std::vector<const char*>& inLayers);

            CHICANE_RUNTIME void init(
                vk::Instance& outInstance,
                vk::detail::DispatchLoaderDynamic& outDldi
            );
        }
    }
}