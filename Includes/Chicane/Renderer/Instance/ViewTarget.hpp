#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER InstanceViewTarget
        {
        public:
            String        name   = "";
            Frame         frame  = {};
            std::uint32_t width  = 0;
            std::uint32_t height = 0;
        };
    }
}
