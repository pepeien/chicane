#pragma once

#include "Chicane/Core/Math/Mat/Mat4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawData3D : DrawData
        {
        public:
            Mat4 model = Mat4::Zero;
        };
    }
}