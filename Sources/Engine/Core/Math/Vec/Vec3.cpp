#include "Chicane/Core/Math/Vec/Vec3.reflected.hpp"

namespace Chicane
{
    String Vec3::toString() const
    {
        return String::sprint("[%.2f, %.2f, %.2f]", x, y, z);
    }

    Vec3 Vec3::cross(const Vec3& inValue) const
    {
        return glm::cross(*this, inValue);
    }
}