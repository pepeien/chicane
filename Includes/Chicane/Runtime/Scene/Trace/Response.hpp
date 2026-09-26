#pragma once

#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class Actor;

    struct CHICANE_RUNTIME SceneTraceResponse
    {
    public:
        Time::Point timestamp = {};
        Vec3        location  = Vec3::sZero();
        Vec3        impact    = Vec3::sZero();
        float       distance  = 0.0f;
        Vec3        start     = Vec3::sZero();
        Vec3        end       = Vec3::sZero();
        Actor*      actor     = nullptr;
    };
}
