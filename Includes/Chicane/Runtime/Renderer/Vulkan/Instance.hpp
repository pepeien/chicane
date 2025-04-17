#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Instance
        {
            CHICANE bool areExtensionsSupported(const std::vector<const char*>& inExtensions);
            CHICANE bool areValidationLayersSupported(const std::vector<const char*>& inValidationLayers);

            CHICANE void init(
                vk::Instance& outInstance,
                vk::DispatchLoaderDynamic& outDldi,
                SDL_Window* inWindow
            );
        }
    }
}