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
            std::vector<Vertex::Instance> vertexInstances;
            std::vector<uint32_t>         vertexIndices;
            Vec<3, float>                 extent;
        };
    }
}