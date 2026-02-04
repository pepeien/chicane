#pragma once

#include <cstdint>

#include "Chicane/Core/Math/Vec.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Settings
        {
        public:
            Vec<2, std::uint32_t> resolution = Vec<2, std::uint32_t>(0U);
        };
    }
}