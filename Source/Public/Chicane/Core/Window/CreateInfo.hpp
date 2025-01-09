#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Window/Type.hpp"
#include "Chicane/Renderer/Type.hpp"

namespace Chicane
{
    namespace Window
    {
        struct CreateInfo
        {
        public:
            std::string    title        = "";
            std::string    icon         = ""; // Optional `file path`
            Vec<2, int>    resolution   = Vec<2, int>(0);
            int            display      = 0;
            Type           type         = Type::Windowed;
            Renderer::Type   renderer     = Renderer::Type::Undefined;
            bool           bIsFocused   = false;
            bool           bIsResizable = true; // Only takes effect when the type is `Type::Windowed`
        };
    }
}