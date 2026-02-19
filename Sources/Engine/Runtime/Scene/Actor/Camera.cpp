#include "Chicane/Runtime/Scene/Actor/Camera.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ACamera::ACamera()
        : Actor(),
          m_camera(nullptr)
    {}

    void ACamera::onLoad()
    {
        m_camera = getScene()->createComponent<CCamera>();
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