#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER TextureStreamerPriority
        {
        public:
            Draw::Id id       = Draw::InvalidId;
            float    screenPx = 0.0f;
            bool     bPinned  = false;
        };
    }
}
