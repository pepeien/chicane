#pragma once

#include <cstdlib>
#include <vector>

#include "Chicane/Core/Image.hpp"
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
            Vertex::List    vertices = {};
            Vertex::Indices indices  = {};
        };
    }
}