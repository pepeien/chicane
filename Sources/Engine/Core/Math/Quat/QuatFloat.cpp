#include "Chicane/Core/Math/Quat/QuatFloat.hpp"

namespace Chicane
{
    QuatFloat QuatFloat::sFromAxis(const Vec3& inAxis, float inAngle)
    {
        return glm::angleAxis(inAngle, static_cast<glm::vec3>(inAxis.normalize()));
    }

    QuatFloat QuatFloat::sFromEuler(const Vec3& inAngles)
    {
        return glm::quat(glm::radians(static_cast<glm::vec3>(inAngles)));
    }

    QuatFloat QuatFloat::sLookAt(const Vec3& inDirection, const Vec3& inUp)
    {
        return glm::quatLookAt(static_cast<glm::vec3>(inDirection), static_cast<glm::vec3>(inUp));
    }

    QuatFloat QuatFloat::normalize() const
    {
        return glm::normalize(static_cast<const glm::quat&>(*this));
    }

    Vec3 QuatFloat::toEuler() const
    {
        return glm::degrees(glm::eulerAngles(static_cast<const glm::quat&>(*this)));
    }
}
