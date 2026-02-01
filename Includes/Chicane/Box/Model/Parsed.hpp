#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX ModelParsed
        {
        public:
            Vertex::List    vertices = {};
            Vertex::Indices indices  = {};
        };
    }
}