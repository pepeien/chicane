#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            struct RawData
            {
            public:
                std::vector<Vertex::Instance> vertices = {};
                std::vector<std::uint32_t>    indices  = {};
            };

            struct CompiledData
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