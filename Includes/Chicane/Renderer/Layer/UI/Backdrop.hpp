#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER UIBackdrop
        {
        public:
            RHI::Image    image  = {};
            std::uint32_t width  = 0;
            std::uint32_t height = 0;
        };
    }
}
