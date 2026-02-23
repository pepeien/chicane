#pragma once

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"
#include "Chicane/Renderer/Draw/Poly/Mode.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPolyData : DrawData
        {
        public:
            DrawPolyMode    mode     = DrawPolyMode::Fill;
            Vertex::List    vertices = {};
            Vertex::Indices indices  = {};
        };
    }
}