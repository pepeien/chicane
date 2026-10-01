#include "Chicane/Runtime/Scene/Actor/Camera.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ACamera::ACamera()
        : Actor(),
          m_camera(nullptr)
    {}

    void ACamera::createDefaultComponents()
    {
        m_camera = createDefaultComponent<CCamera>("Camera");
    }

    void ACamera::activate()
    {
        if (m_camera)
        {
            m_camera->activate();
        }
    }

    void ACamera::deactivate()
    {
        if (m_camera)
        {
            m_camera->deactivate();
        }
    }
}
