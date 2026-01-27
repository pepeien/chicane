#include "Chicane/Core/Math/Rotator.hpp"

namespace Chicane
{
    const QuatFloat& Rotator::getOrientation() const
    {
        return m_orientation;
    }

    void Rotator::setOrientation(const QuatFloat& inValue)
    {
        m_orientation = inValue;

        refresh();
    }

    const Vec3& Rotator::getVector() const
    {
        return m_euler;
    }

    void Rotator::add(const Vec3& inRotation)
    {
        QuatFloat delta = inRotation.toQuat();

        m_orientation = m_orientation * delta;

        refresh();
    }

    void Rotator::set(const Vec3& inRotation)
    {
        m_orientation = inRotation.toQuat();

        refresh();
    }

    const Vec3& Rotator::getForward() const
    {
        return m_forward;
    }

    const Vec3& Rotator::getUp() const
    {
        return m_up;
    }

    const Vec3& Rotator::getRight() const
    {
        return m_right;
    }

    void Rotator::refresh()
    {
        m_orientation = glm::normalize(m_orientation);

        m_euler = glm::degrees(glm::eulerAngles(m_orientation));

        m_forward = m_orientation * Vec3::Forward;
        m_up      = m_orientation * Vec3::Up;
        m_right   = m_orientation * Vec3::Right;
    }
}