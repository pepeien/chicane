#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Depth/Compare.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Depth
        {
        public:
            bool         bCanWrite = true;

            DepthCompare compare = DepthCompare::Always;
        };
    }
}