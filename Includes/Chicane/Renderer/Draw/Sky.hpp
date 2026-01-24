#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawSky : Draw
        {
        public:
            DrawPoly          model    = {};
            DrawTexture::List textures = {}; // [Right, Left, Front, Back, Up, Down]
        };
    }
}