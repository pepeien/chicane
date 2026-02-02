#pragma once

#include "Chicane/Core/Math/Vec.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Settings
        {
        public:
            Vec<2, int> resolution = Vec<2, int>(-1);
        };
    }
}