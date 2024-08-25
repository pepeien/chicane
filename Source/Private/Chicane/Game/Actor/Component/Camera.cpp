#include "Chicane/Game/Actor/Component/Camera.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    CameraComponent::CameraComponent()
        : ActorComponent(),
        m_camera(std::make_unique<Camera>())
    {}

    void CameraComponent::onActivation()
    {
        setActiveCamera(m_camera.get());
    }

    void CameraComponent::onDeactivation()
    {
        if (getActiveCamera() != m_camera.get())
        {
            return;
        }

        setActiveCamera(nullptr);
    }

    void CameraComponent::onAttachment()
    {
        m_owner->setAbsoluteTranslation(Vec<3, float>(0.0f, -110.0f, 50.0f));
    }

    void CameraComponent::onTick(float inDeltaTime)
    {
        m_camera->setTranslation(m_transform.translation);
        m_camera->setRotation(m_transform.rotation);
    }
}