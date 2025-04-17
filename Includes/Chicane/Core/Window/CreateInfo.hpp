#pragma once

#include "Core/Essentials.hpp"
#include "Core/Math/Vec.hpp"
#include "Core/Window/Type.hpp"

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