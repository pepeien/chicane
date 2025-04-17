#pragma once

#include "Core.hpp"
#include "Core/Window/CreateInfo.hpp"
#include "Runtime/Renderer/CreateInfo.hpp"

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