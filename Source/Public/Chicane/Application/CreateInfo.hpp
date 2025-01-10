#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window.hpp"

namespace Chicane
{
    namespace Application
    {
        struct CreateInfo : public Window::CreateInfo
        {
        public:
            // Callbacks
            std::function<void ()> onSetup = nullptr;
        };
    }
}