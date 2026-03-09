#include "Chicane/Core/Math/Rotator.reflected.hpp"

namespace Chicane
{
    static const QuatFloat CORRECTION = glm::angleAxis(glm::radians(-90.0f), static_cast<glm::vec3>(Vec3::Right));

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
          angles(Vec3::Zero),
          right(Vec3::Right),
          forward(Vec3::Forward),
          up(Vec3::Up)
    {}

    const QuatFloat& Rotator::get() const
    {
        return m_orientation;
    }

    void Rotator::set(float inValue)
    {
        set(Rotator(inValue).get());
    }

    void Rotator::set(float inPitch, float inRoll, float inYaw)
    {
        set(Rotator(inPitch, inRoll, inYaw).get());
    }

    void Rotator::set(const Vec3& inValue)
    {
        set(Rotator(inValue).get());
    }

    void Rotator::set(const Rotator& inValue)
    {
        set(inValue.get());
    }

    void Rotator::set(const QuatFloat& inValue)
    {
        m_orientation = inValue;

        refresh();
    }

    void Rotator::addLocal(float inValue)
    {
        addLocal(Rotator(inValue).get());
    }

    void Rotator::addLocal(float inPitch, float inRoll, float inYaw)
    {
        addLocal(Rotator(inPitch, inRoll, inYaw).get());
    }

    void Rotator::addLocal(const Vec3& inValue)
    {
        addLocal(Rotator(inValue).get());
    }

    void Rotator::addLocal(const Rotator& inValue)
    {
        addLocal(inValue.get());
    }

    void Rotator::addLocal(const QuatFloat& inDelta)
    {
        m_orientation = glm::normalize(m_orientation * inDelta);

        refresh();
    }

    void Rotator::addWorld(float inValue)
    {
        addWorld(Rotator(inValue).get());
    }

    void Rotator::addWorld(float inPitch, float inRoll, float inYaw)
    {
        addWorld(Rotator(inPitch, inRoll, inYaw).get());
    }

    void Rotator::addWorld(const Vec3& inValue)
    {
        addWorld(Rotator(inValue).get());
    }

    void Rotator::addWorld(const Rotator& inValue)
    {
        addWorld(inValue.get());
    }

    void Rotator::addWorld(const QuatFloat& inDelta)
    {
        m_orientation = glm::normalize(inDelta * m_orientation);

        refresh();
    }

    void Rotator::lookAt(const Vec3& inOrigin, const Vec3& inTarget)
    {
        const Vec3 direction = glm::normalize(static_cast<glm::vec3>(inTarget) - static_cast<glm::vec3>(inOrigin));

        m_orientation = glm::normalize(
            glm::quatLookAt(static_cast<glm::vec3>(direction), static_cast<glm::vec3>(Vec3::Up)) * CORRECTION
        );

        refresh();
    }

    const Vec3& Rotator::getAngles() const
    {
        return angles;
    }

    void Rotator::setAngles(const Vec3& inAngles)
    {
        m_orientation = glm::quat(glm::radians(glm::vec3(inAngles.x, inAngles.y, inAngles.z)));

        refresh();
    }

    const Vec3& Rotator::getRight() const
    {
        return right;
    }

    const Vec3& Rotator::getForward() const
    {
        return forward;
    }

    const Vec3& Rotator::getUp() const
    {
        return up;
    }

    void Rotator::refresh()
    {
        angles = glm::degrees(glm::eulerAngles(m_orientation));

        right   = m_orientation * static_cast<glm::vec3>(Vec3::Right);
        forward = m_orientation * static_cast<glm::vec3>(Vec3::Forward);
        up      = m_orientation * static_cast<glm::vec3>(Vec3::Up);
    }
}