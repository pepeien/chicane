#include "Chicane/Game/Components/Camera.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    CameraComponent::CameraComponent()
        : ActorComponent(),
        m_willFollowOwner(false),
        m_camera(std::make_unique<Camera>())
    {}

    void CameraComponent::activate()
    {
        setCamera(m_camera.get());
    }

    void CameraComponent::onAttachment()
    {
        m_owner->setAbsoluteTranslation(Vec<3, float>(0.0f, -110.0f, 50.0f));
        m_owner->setAbsoluteRotation(Vec<3, float>(0.0f, 0.0f, -30.0f));
    }

    void CameraComponent::onTick(float inDeltaTime)
    {
        refreshPosition();
    }

    bool CameraComponent::willFollowOwner() const
    {
        return m_willFollowOwner;
    }

    void CameraComponent::setWillFollowOwner(bool inWillFollowOwner)
    {
        m_willFollowOwner = inWillFollowOwner;
    }

    void CameraComponent::refreshPosition()
    {
        if (!willFollowOwner() || !hasOwner())
        {
            return;
        }

        m_camera->setTranslation(m_owner->getTranslation());
    }
}