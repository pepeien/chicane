#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX ModelExtracted
        {
        public:
            std::vector<Vertex>        vertices = {};
            std::vector<std::uint32_t> indices  = {};
        };
    }
}