#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
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
            Draw::Id           model    = Draw::UnknownId;
            DrawPoly3DInstance instance = {};
        };
    }
}