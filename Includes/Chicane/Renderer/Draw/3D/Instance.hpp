#pragma once

#include "Chicane/Core/Math/Mat/Mat4.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw3DInstance
        {
        public:
            Mat4 model = Mat4::Zero;
        };
    }
}