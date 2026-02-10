#include "Chicane/Runtime/Scene/Actor/Camera.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    ACamera::ACamera()
        : Actor(),
          m_camera(nullptr)
    {
        m_camera = Application::getInstance().getScene()->createComponent<CCamera>();
        m_camera->attachTo(this);
    }

    void ACamera::activate()
    {
        m_camera->activate();
    }

    void ACamera::deactivate()
    {
        m_camera->deactivate();
    }
}