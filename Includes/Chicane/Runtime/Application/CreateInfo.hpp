#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Window/CreateInfo.hpp"
#include "Chicane/Runtime/Renderer/CreateInfo.hpp"

namespace Chicane
{
    namespace Application
    {
        struct CHICANE CreateInfo
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