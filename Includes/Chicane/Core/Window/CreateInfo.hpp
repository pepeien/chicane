#pragma once

#include "Chicane/Core/Essentials.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Window/Type.hpp"

namespace Chicane
{
    namespace Window
    {
        struct CHICANE CreateInfo
        {
        public:
            std::string    title   = "";
            std::string    icon    = ""; // Optional `file path`
            Vec<2, int>    size    = Vec<2, int>(0);
            int            display = 0;
            Type           type    = Type::Windowed;
        };
    }
}