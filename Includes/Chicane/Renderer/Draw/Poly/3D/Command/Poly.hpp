#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DCommandPoly
        {
        public:
            using List = std::vector<DrawPoly3DCommandPoly>;

        public:
            DrawPolyData       data     = {};
            DrawPoly3DInstance instance = {};
        };
    }
}
