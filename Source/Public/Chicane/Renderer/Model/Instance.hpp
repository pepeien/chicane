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
            std::vector<Vertex::Instance> vertices;
            std::vector<std::uint32_t>    indices;
        };
    }
}