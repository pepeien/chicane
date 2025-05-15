#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Instance
        {
            CHICANE_RUNTIME bool areExtensionsSupported(const std::vector<const char*>& inExtensions);
            CHICANE_RUNTIME bool areValidationLayersSupported(const std::vector<const char*>& inValidationLayers);

            CHICANE_RUNTIME void init(
                vk::Instance& outInstance,
                vk::DispatchLoaderDynamic& outDldi,
                SDL_Window* inWindow
            );
        }
    }
}