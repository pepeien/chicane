#pragma once

#include <cstdlib>
#include <vector>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw
        {
        public:
            using Id = int;

        public:
            static inline constexpr const Id UNKNOWN_ID = -1;

        public:
            Id            id          = UNKNOWN_ID;
            std::uint32_t vertexStart = 0U;
            std::uint32_t vertexCount = 0U;
            std::uint32_t indexStart  = 0U;
            std::uint32_t indexCount  = 0U;
        };
    }
}