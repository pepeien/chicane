#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    const Vec4 Vec4::Zero    = Vec4();

    const Vec4 Vec4::Up      = Vec4(Vec3::Up,      0.0f);
    const Vec4 Vec4::Right   = Vec4(Vec3::Right,   0.0f);
    const Vec4 Vec4::Forward = Vec4(Vec3::Forward, 0.0f);
}