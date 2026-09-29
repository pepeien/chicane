#pragma once

#include <array>
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Light/Scene.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER ShadowLight
        {
        public:
            Vec4                                   splits = Vec4::sZero();
            Vec4                                   info   = Vec4::sZero();

            std::array<SceneLight, MAX_LIGHTS>     lights      = {};
            std::array<Mat4, SHADOW_CASCADE_COUNT> views       = {};
            std::array<Mat4, SHADOW_CASCADE_COUNT> projections = {};
        };
    }
}
