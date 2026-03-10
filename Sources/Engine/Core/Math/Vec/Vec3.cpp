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
        return String::sprint("[%.2f, %.2f, %.2f]", x, y, z);
    }

    Vec3 Vec3::min(const Vec3& inValue) const
    {
        return glm::min(static_cast<glm::vec3>(*this), static_cast<glm::vec3>(inValue));
    }

    Vec3 Vec3::max(const Vec3& inValue) const
    {
        return glm::max(static_cast<glm::vec3>(*this), static_cast<glm::vec3>(inValue));
    }

    Vec3 Vec3::cross(const Vec3& inValue) const
    {
        return glm::cross(static_cast<glm::vec3>(*this), static_cast<glm::vec3>(inValue));
    }

    float Vec3::dot(const Vec3& inValue) const
    {
        return glm::dot(static_cast<glm::vec3>(*this), static_cast<glm::vec3>(inValue));
    }

    Vec3 Vec3::normalize() const
    {
        return glm::normalize(static_cast<glm::vec3>(*this));
    }
}