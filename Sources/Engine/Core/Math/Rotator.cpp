#include "Chicane/Core/Math/Rotator.hpp"

namespace Chicane
{
    Rotator::Rotator(float inAngle)
        : Rotator(inAngle, inAngle, inAngle)
    {}

    Rotator::Rotator(float inPitch, float inRoll, float inYaw)
        : Rotator(Vec3(inPitch, inRoll, inYaw))
    {}

    Rotator::Rotator(const Vec3& inAngles)
        : Rotator()
    {
        setAngles(inAngles);
    }

    Rotator::Rotator(const QuatFloat& inOrientation)
        : Rotator()
    {
        set(inOrientation);
    }

    Rotator::Rotator()
        : m_orientation(QuatFloat(1, 0, 0, 0)),
          m_angles(Vec3::Zero),
          m_right(Vec3::Right),
          m_forward(Vec3::Forward),
          m_up(Vec3::Up)
    {}

    const QuatFloat& Rotator::get() const
    {
        return m_orientation;
    }

    void Rotator::set(const QuatFloat& inValue)
    {
        m_orientation = inValue;

        refresh();
    }

    void Rotator::addLocal(const QuatFloat& inDelta)
    {
        set(glm::normalize(m_orientation * inDelta));
    }

    void Rotator::addWorld(const QuatFloat& inDelta)
    {
        set(glm::normalize(inDelta * m_orientation));
    }

    const Vec3& Rotator::getAngles() const
    {
        return m_angles;
    }

    void Rotator::setAngles(const Vec3& inAngles)
    {
        set(glm::quat(glm::radians(glm::vec3(inAngles.x, inAngles.y, inAngles.z))));
    }

    const Vec3& Rotator::getRight() const
    {
        return m_right;
    }

    const Vec3& Rotator::getForward() const
    {
        return m_forward;
    }

    const Vec3& Rotator::getUp() const
    {
        return m_up;
    }

    void Rotator::refresh()
    {
        m_angles = glm::degrees(glm::eulerAngles(m_orientation));

        m_right   = m_orientation * Vec3::Right;
        m_forward = m_orientation * Vec3::Forward;
        m_up      = m_orientation * Vec3::Up;
    }
}