#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX ModelParsed
        {
        public:
            std::uint32_t vertexCount   = 0;
            std::uint32_t firstVertex   = 0;
            std::uint32_t firstInstance = 0;
            std::uint32_t indexCount    = 0;
            std::uint32_t firstIndex    = 0;
        };
    }
}