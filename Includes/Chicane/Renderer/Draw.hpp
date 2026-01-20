#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename I>
        struct CHICANE_RENDERER Draw
        {
        public:
            using List      = std::vector<Draw<I>>;
            using Instances = std::vector<I>;

        public:
            std::uint32_t vertexStart = 0U;
            std::uint32_t vertexCount = 0U;
            std::uint32_t indexStart  = 0U;
            std::uint32_t indexCount  = 0U;
            Instances     instances   = {};
        };
    }
}