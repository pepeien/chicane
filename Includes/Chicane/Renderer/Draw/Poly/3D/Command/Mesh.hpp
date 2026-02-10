#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DCommandMesh
        {
        public:
            using List = std::vector<DrawPoly3DCommandMesh>;

        public:
            String model   = {};
            String texture = {};
        };
    }
}