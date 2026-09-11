#pragma once

#include <vector>

#include "Chicane/Core/Math/Quat/QuatFloat.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    namespace Box
    {
        struct SamplerCurve
        {
            std::vector<float>     times;
            std::vector<Vec3>      vectors;
            std::vector<QuatFloat> rotations;
            bool                   step = false;
        };

    }
}
