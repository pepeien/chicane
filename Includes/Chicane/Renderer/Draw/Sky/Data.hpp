#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawSkyData : DrawData
        {
        public:
            Draw::Reference     model    = "";
            Draw::ReferenceList textures = {};
        };
    }
}