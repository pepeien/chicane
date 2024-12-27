#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Window/Type.hpp"

namespace Chicane
{
    namespace Window
    {
        struct CreateInfo
        {
        public:
            std::string title        = "";
            std::string icon         = ""; // Optional [file path]
            Vec<2, int> resolution   = Vec<2, int>(0);
            int         displayIndex = 0;
            Type        type         = Type::Windowed;
            bool        bIsFocused   = false;
            bool        bIsResizable = true; // Only takes effect when the type is `Type::Windowed`
        };
    }
}