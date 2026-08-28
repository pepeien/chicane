#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Target
        {
        public:
            using Id = Draw::Id;

        public:
            Draw::Reference   reference = Draw::InvalidReference;
            std::uint32_t     width     = 0;
            std::uint32_t     height    = 0;
        };
    }
}
