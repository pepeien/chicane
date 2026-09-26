#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Object;

    struct CHICANE_RUNTIME SceneSpatialCell
    {
    public:
        Vec3                 min     = Vec3::sZero();
        Vec3                 max     = Vec3::sZero();
        std::vector<Object*> objects = {};
    };
}
