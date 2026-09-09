#pragma once

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
            Vec4       splits = Vec4::Zero();
            Vec4       info   = Vec4::Zero();

            Mat4       views[SHADOW_CASCADE_COUNT]       = {};
            Mat4       projections[SHADOW_CASCADE_COUNT] = {};

            SceneLight lights[MAX_LIGHTS] = {};
        };

        static_assert(
            sizeof(ShadowLight) ==
                (2 * sizeof(Vec4)) + (2 * SHADOW_CASCADE_COUNT * sizeof(Mat4)) + (MAX_LIGHTS * sizeof(SceneLight)),
            "ShadowLight must stay tightly packed for GPU upload"
        );
    }
}
