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
                m_camera->setAbsoluteTranslation(inTransform.translation);
                m_camera->setAbsoluteRotation(inTransform.rotation);
            }
        );
    }

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
}