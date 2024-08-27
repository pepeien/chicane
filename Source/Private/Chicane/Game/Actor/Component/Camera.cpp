#include "Chicane/Game/Actor/Component/Camera.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    CameraComponent::CameraComponent()
        : ActorComponent(),
        m_camera(std::make_unique<Camera>())
    {
        watchTransform(
            [this](const Transform& inTransform)
            {
                refreshCamera();
            }
        );

        refreshCamera();
    }

    void CameraComponent::onActivation()
    {
        refreshCamera();

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

    void CameraComponent::refreshCamera()
    {
        m_camera->setAbsoluteTranslation(m_transform.translation);
        m_camera->setAbsoluteRotation(m_transform.rotation);
    }
}