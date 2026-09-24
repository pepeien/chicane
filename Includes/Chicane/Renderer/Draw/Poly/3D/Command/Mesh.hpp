#pragma once

#include <array>
#include <vector>

#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Texture/Material.hpp"

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
            Draw::Reference                                     model    = Draw::InvalidReference;
            std::array<Draw::Reference, TEXTURE_MATERIAL_COUNT> textures = {};
            DrawPoly3DInstance                                  instance = {};
        };
    }
}
