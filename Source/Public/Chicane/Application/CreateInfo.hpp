#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window/CreateInfo.hpp"
#include "Chicane/Renderer/CreateInfo.hpp"

namespace Chicane
{
    namespace Application
    {
        struct CreateInfo
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