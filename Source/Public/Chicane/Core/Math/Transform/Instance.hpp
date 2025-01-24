#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Transform
    {
        struct Instance
        {
        public:
            Vec<3, float> translation = Vec3Zero;
            Vec<3, float> rotation    = Vec3Zero;
            Vec<3, float> scale       = Vec<3, float>(1.0f);
        };
    }
}