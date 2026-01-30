#pragma once

#include <map>
#include <vector>

#include "Chicane/Core/Math/Mat/Mat4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawPoly3DInstance
        {
        public:
            using List = std::vector<DrawPoly3DInstance>;
            using Map  = std::map<Draw::Id, List>;

        public:
            Mat4         model   = Mat4::Zero;
            Draw::Id     texture = -1;

            std::int32_t _padding[3]; // Shader alignment
        };
    }
}