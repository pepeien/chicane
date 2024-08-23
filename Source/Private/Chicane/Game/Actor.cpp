#include "Chicane/Game/Actor.hpp"

#include "Chicane/Game/Components/Actor.hpp"
#include "Chicane/Game/Components/Camera.hpp"

namespace Chicane
{
    Actor::Actor()
        : m_bCanTick(false),
        m_position(Mat<4, float>(1.0f)),
        m_transform({}),
        m_camera(nullptr)
    {}

    Actor::~Actor()
    {
        delete m_camera;
    }

    bool Actor::canTick() const
    {
        return m_bCanTick;
    }

    void Actor::setCanTick(bool bInCanTick)
    {
        glm::vec2(1.0f);
        m_bCanTick = bInCanTick;
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

    bool Actor::hasCamera() const
    {
        return m_camera != nullptr;
    }

    const std::vector<ActorComponent*>& Actor::getComponents() const
    {
        return m_components;
    }

    bool Actor::hasMesh() const
    {
        return !m_mesh.name.empty();
    }

    const Box::Instance& Actor::getMesh() const
    {
        return m_mesh;
    }

    void Actor::updateTranslation(const Vec<3, float>& inTranslation)
    {
        bool didChange = inTranslation.x != m_transform.translation.x ||
                         inTranslation.y != m_transform.translation.y ||
                         inTranslation.z != m_transform.translation.z;

        if (!didChange)
        {
            return;
        }

        m_position = glm::translate(m_position, inTranslation);

        m_transform.translation = inTranslation;
    }

    void Actor::updateRotation(const Vec<3, float>& inRotation)
    {
        bool didChange = inRotation.x != m_transform.rotation.x ||
                         inRotation.y != m_transform.rotation.y ||
                         inRotation.z != m_transform.rotation.z;

        if (!didChange)
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
        bool didChange = inScale.x != m_transform.scale.x ||
                         inScale.y != m_transform.scale.y ||
                         inScale.z != m_transform.scale.z;

        if (!didChange)
        {
            return;
        }

        m_position = glm::scale(m_position, inScale);

        m_transform.scale = inScale;
    }
}