#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER ViewportSettings
        {
        public:
            String width   = "100%";
            String height  = "100%";
            String offsetX = "0px";
            String offsetY = "0px";
        };
    }
}