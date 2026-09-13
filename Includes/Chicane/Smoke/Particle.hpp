#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Smoke.hpp"

namespace Chicane
{
    namespace Smoke
    {
        struct CHICANE_SMOKE Particle
        {
        public:
            using List = std::vector<Particle>;

        public:
            Vec3  position     = Vec3::Zero();
            Vec3  velocity     = Vec3::Zero();
            Vec3  axis         = Vec3::Zero();
            Vec2  size         = Vec2(1.0f);
            Vec2  sizeStart    = Vec2(1.0f);
            Vec2  sizeEnd      = Vec2(1.0f);
            Vec4  color        = Vec4(1.0f);
            Vec4  colorStart   = Vec4(1.0f);
            Vec4  colorEnd     = Vec4(1.0f);
            float rotation     = 0.0f;
            float age          = 0.0f;
            float lifetime     = 1.0f;
            float additive     = 1.0f;
            bool  bAlive       = true;
        };
    }
}
