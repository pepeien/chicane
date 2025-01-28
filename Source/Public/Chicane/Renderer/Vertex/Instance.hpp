#pragma once

#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
        public:
            Vec<3, float> position = Vec3Zero;
            Vec<3, float> color    = Vec3Zero;
            Vec<2, float> UV       = Vec2Zero;
            Vec<3, float> normal   = Vec3Zero;
        };
    }
}