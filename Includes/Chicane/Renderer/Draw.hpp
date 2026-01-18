#pragma once

#include <cstdlib>
#include <vector>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw
        {
        public:
            using Id = std::uint32_t;

        public:
            std::uint32_t vertexStart = 0U;
            std::uint32_t vertexCount = 0U;
            std::uint32_t indexStart  = 0U;
            std::uint32_t indexCount  = 0U;
        };
    }
}