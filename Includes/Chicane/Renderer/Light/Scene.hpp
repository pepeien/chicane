#pragma once

#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER SceneLight
        {
        public:
            Vec4 translation = Vec4::Zero();
            Vec4 direction   = Vec4::Zero();
            Vec4 color       = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        };
    }
}
