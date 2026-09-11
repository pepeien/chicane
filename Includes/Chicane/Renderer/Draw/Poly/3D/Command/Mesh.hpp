#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Texture/Map.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DCommandMesh
        {
        public:
            using List = std::vector<DrawPoly3DCommandMesh>;

        public:
            Draw::Reference    model                       = Draw::InvalidReference;
            Draw::Reference    textures[TEXTURE_MAP_COUNT] = {};
            DrawPoly3DInstance instance                    = {};
        };
    }
}
