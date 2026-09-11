#pragma once

#include <Chicane/Core/Math/Vec/Vec3.hpp>

namespace Editor
{
    struct StudioLight
    {
    public:
        Chicane::Vec3 color = {};
        Chicane::Vec3 view  = {};
    };
}
