#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Instance
    {
        bool areExtensionsSupported(const std::vector<const char*> inExtensions);
        bool areValidationLayersSupported(const std::vector<const char*> inValidationLayers);

        void init(
            vk::Instance& outInstance,
            vk::DispatchLoaderDynamic& outDldi,
            SDL_Window* inWindow
        );
    }
}