#pragma once

#include "Chicane/Core/Math/Mat/Mat4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw3D : Draw
        {
        public:
            using List = std::vector<Draw3D>;

        public:
            Mat4 model = Mat4::Zero;
        };
    }
}