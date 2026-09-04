#include "Chicane/Core/Math/Quat/QuatFloat.hpp"

namespace Chicane
{
    QuatFloat QuatFloat::fromAxis(const Vec3& inAxis, float inAngle)
    {
        return glm::angleAxis(inAngle, static_cast<glm::vec3>(inAxis.normalize()));
    }

    QuatFloat QuatFloat::normalize() const
    {
        return glm::normalize(static_cast<const glm::quat&>(*this));
    }
}
