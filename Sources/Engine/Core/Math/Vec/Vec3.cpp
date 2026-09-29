#include "Chicane/Core/Math/Vec/Vec3.reflected.hpp"

#include "Chicane/Core/Math/Vec/Vec4.hpp"

namespace Chicane
{
    constexpr Vec3::Vec3(const Vec4& inValue)
        : x(inValue.x),
          y(inValue.y),
          z(inValue.z)
    {}

    String Vec3::toString() const
    {
        return String::sSprint("[%.2f, %.2f, %.2f]", x, y, z);
    }
}