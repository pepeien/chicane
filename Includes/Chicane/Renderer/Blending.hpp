#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Blending/Factor.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Blending
        {
        public:
            BlendingFactor source      = BlendingFactor::Zero;
            BlendingFactor destination = BlendingFactor::One;
        };
    }
}