#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            struct CHICANE_BOX Extracted
            {
            public:
                std::vector<Vertex>        vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };
        }
    }
}