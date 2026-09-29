#pragma once

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Smoke.hpp"

namespace Chicane
{
    namespace Smoke
    {
        struct CHICANE_SMOKE PlayInfo
        {
        public:
            Transform transform   = {};
            Vec3      origin      = Vec3::sZero();
            Vec3      destination = Vec3::sZero();
            bool      bHasBeam    = false;
        };
    }
}
