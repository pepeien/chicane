#pragma once

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawData
        {
        public:
            String          reference = "";
            Vertex::List    vertices  = {};
            Vertex::Indices indices   = {};
        };
    }
}