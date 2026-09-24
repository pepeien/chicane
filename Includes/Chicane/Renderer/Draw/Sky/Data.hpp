#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"
#include "Chicane/Renderer/Draw/Sky/Kind.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawSkyData : public DrawData
        {
        public:
            Draw::Reference     model    = "";
            Draw::ReferenceList textures = {};
            DrawSkyKind         kind     = DrawSkyKind::Cube;
            float               exposure = 1.0f;
            bool                bVisible = true;
        };
    }
}