#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Light/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Light
        {
        public:
            using List = std::vector<Light>;

        public:
            LightType type        = LightType::Directional;
            bool      castShadows = false;
            Vec3      color       = Vec3(1.0f);
            float     intensity   = 1.0f;
            float     range       = 50.0f;
            Vec3      translation = Vec3::Zero();
            Vec3      direction   = Vec3::Forward();
            float     innerAngle  = 25.0f;
            float     outerAngle  = 40.0f;
        };
    }
}
