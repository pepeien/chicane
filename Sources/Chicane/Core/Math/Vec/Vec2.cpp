#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    const Vec2 Vec2::Zero = Vec2(0.0f);
    const Vec2 Vec2::One  = Vec2(1.0f);

    const Vec2 Vec2::Up    = Vec2(Vec3::Up.x,    Vec3::Up.y);
    const Vec2 Vec2::Right = Vec2(Vec3::Right.x, Vec3::Right.y);
}