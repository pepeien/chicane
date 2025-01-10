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
            std::string    title      = "";
            std::string    icon       = ""; // Optional `file path`
            Vec<2, int>    resolution = Vec<2, int>(0);
            int            display    = 0;
            Type           window     = Type::Windowed;
            Renderer::Type renderer   = Renderer::Type::Undefined;
        };
    }
}