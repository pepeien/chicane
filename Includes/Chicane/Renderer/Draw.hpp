#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw
        {
        public:
            using Id   = int;
            using List = std::vector<Draw>;

        public:
            static inline constexpr const Id UnknownId = -1;

        public:
            Id            id          = UnknownId;
            String        reference   = "";
            std::uint32_t vertexStart = 0U;
            std::uint32_t vertexCount = 0U;
            std::uint32_t indexStart  = 0U;
            std::uint32_t indexCount  = 0U;
        };
    }
}