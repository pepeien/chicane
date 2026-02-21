#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly2DCommand
        {
        public:
            using List = std::vector<DrawPoly2DCommand>;

        public:
            Renderer::DrawPolyData       polygon  = {};
            Renderer::DrawPoly2DInstance instance = {};
        };
    }
}