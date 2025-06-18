#pragma once

#include "Chicane/Box/Essential.hpp"

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