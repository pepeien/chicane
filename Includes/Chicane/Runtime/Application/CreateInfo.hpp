#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Application
    {
        struct CHICANE_RUNTIME CreateInfo
        {
        public:
            // Renderer
            Renderer::CreateInfo   renderer = {};

            // Window
            Window::CreateInfo     window   = {};

            // Callbacks
            std::function<void ()> onSetup  = nullptr;
        };
    }
}