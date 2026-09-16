#include "Chicane/Runtime/Scene/Actor/Camera.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ACamera::ACamera()
        : Actor(),
          lookAt(),
          m_camera(nullptr)
    {}

    void ACamera::onLoad()
    {
        m_camera = getScene()->createComponent<CCamera>();
        m_camera->attachTo(this);

        applyLookAt(lookAt);
    }

    void ACamera::onPropertyEdited(const String& inName)
    {
        if (inName.equals(LOOK_AT_ATTRIBUTE_NAME))
        {
            applyLookAt(lookAt);
        }
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