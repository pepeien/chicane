#include "Chicane/Game/Actor.hpp"

#include "Chicane/Game/Components/Actor.hpp"
#include "Chicane/Game/Components/Camera.hpp"

namespace Chicane
{
    Actor::Actor()
        : m_canTick(false),
        m_position(Mat<4, float>(1.0f)),
        m_transform({})
    {}

    bool Actor::canTick() const
    {
        return m_canTick;
    }

    void Actor::setCanTick(bool inCanTick)
    {
        m_canTick = inCanTick;
    }

    const Vec<3, float>& Actor::getTranslation() const
    {
        return m_transform.translation;
    }

    const Vec<3, float>& Actor::getRotation() const
    {
        return m_transform.rotation;
    }

    const Mat<4, float>& Actor::getPosition() const
    {
        return m_position;
    }

    void Actor::setRelativeTranslation(const Vec<3, float>& inTranslation)
    {
        Vec<3, float> translation = m_transform.translation;
        translation.x += inTranslation.x;
        translation.y += inTranslation.y;
        translation.z += inTranslation.z;

        updateTranslation(translation);
    }

    void Actor::setAbsoluteTranslation(const Vec<3, float>& inTranslation)
    {
        updateTranslation(inTranslation);
    }

    void Actor::setRelativeRotation(const Vec<3, float>& inRotation)
    {
        Vec<3, float> rotation = m_transform.rotation;
        rotation.x += inRotation.x;
        rotation.y += inRotation.y;
        rotation.z += inRotation.z;

        updateRotation(rotation);
    }

    void Actor::setAbsoluteRotation(const Vec<3, float>& inRotation)
    {
        updateRotation(inRotation);
    }

    void Actor::setRelativeScale(const Vec<3, float>& inScale)
    {
        Vec<3, float> scale = m_transform.scale;
        scale.x += inScale.x;
        scale.y += inScale.y;
        scale.z += inScale.z;

        updateScale(scale);
    }

    void Actor::setAbsoluteScale(const Vec<3, float>& inScale)
    {
        updateScale(inScale);
    }

    bool Actor::hasMesh() const
    {
        return !m_mesh.name.empty();
    }

    const Box::Instance& Actor::getMesh() const
    {
        return m_mesh;
    }
  
    void Actor::setMesh(const Box::Instance& inMesh)
    {
        m_mesh = inMesh;
    }

    void Actor::updateTranslation(const Vec<3, float>& inTranslation)
    {
        bool isIdentical = std::fabs(m_transform.translation.x - inTranslation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.y - inTranslation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.z - inTranslation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_position = glm::translate(m_position, inTranslation);

        m_transform.translation = inTranslation;
    }

    void Actor::updateRotation(const Vec<3, float>& inRotation)
    {
        bool isIdentical = std::fabs(inRotation.x != m_transform.rotation.x) < FLT_EPSILON &&
                           std::fabs(inRotation.y != m_transform.rotation.y) < FLT_EPSILON &&
                           std::fabs(inRotation.z != m_transform.rotation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_position = glm::rotate(
            m_position,
            glm::radians(inRotation.x),
            FORWARD_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            glm::radians(inRotation.y),
            RIGHT_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            glm::radians(inRotation.z),
            UP_DIRECTION
        );

        m_transform.rotation = inRotation;
    }

    void Actor::updateScale(const Vec<3, float>& inScale)
    {
        bool isIdentical = std::fabs(m_transform.scale.x - inScale.x) < FLT_EPSILON &&
                           std::fabs(m_transform.scale.y - inScale.y) < FLT_EPSILON &&
                           std::fabs(m_transform.scale.z - inScale.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_position = glm::scale(m_position, inScale);

        m_transform.scale = inScale;
    }
}