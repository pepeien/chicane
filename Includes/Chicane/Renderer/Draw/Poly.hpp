#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly : Draw
        {
        public:
            using List = std::vector<DrawPoly>;
            using Map  = std::map<DrawPolyType, List>;

        public:
            std::uint32_t vertexStart   = 0U;
            std::uint32_t vertexCount   = 0U;
            std::uint32_t indexStart    = 0U;
            std::uint32_t indexCount    = 0U;
            std::uint32_t instanceStart = 0U;
            std::uint32_t instanceCount = 0U;
        };
    }
}