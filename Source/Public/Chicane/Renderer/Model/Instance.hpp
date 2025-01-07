#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Model
    {
        struct Instance
        {
        public:
            Bounds                        bounds;

            std::vector<Vertex::Instance> vertexInstances;
            std::vector<std::uint32_t>    vertexIndices;
        };
    }
}