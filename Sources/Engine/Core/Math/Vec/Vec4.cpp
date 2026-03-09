#include "Chicane/Core/Math/Vec/Vec4.reflected.hpp"

namespace Chicane
{
    const Vec4 Vec4::Zero = Vec4(0.0f);
    const Vec4 Vec4::One  = Vec4(1.0f);

    const Vec4 Vec4::Right   = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    const Vec4 Vec4::Forward = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
    const Vec4 Vec4::Up      = Vec4(0.0f, 0.0f, 1.0f, 0.0f);

    String Vec4::toString() const
    {
        return String::sprint("[%.2f, %.2f, %.2f, %2f]", x, y, z, w);
    }
}