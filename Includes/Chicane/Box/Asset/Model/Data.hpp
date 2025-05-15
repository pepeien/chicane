#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            struct CHICANE_BOX RawData
            {
            public:
                std::vector<Vertex>        vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };

            struct CHICANE_BOX CompiledData
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
}