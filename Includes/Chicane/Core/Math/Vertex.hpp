#pragma once

#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vertex
    {
    public:
        friend bool operator==(const Vertex& inLeft, const Vertex& inRight)
        {
            return (
                inLeft.position == inRight.position &&
                inLeft.color    == inRight.color &&
                inLeft.uv       == inRight.uv &&
                inLeft.normal   == inRight.normal
            );
        }

    public:
        Vec3 position = Vec3::Zero;
        Vec4 color    = Vec4::Zero;
        Vec2 uv       = Vec2::Zero;
        Vec3 normal   = Vec3::Zero;
    };
}