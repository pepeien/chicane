#include "Chicane/Runtime/Scene/Actor/Camera.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ACamera::ACamera()
        : Actor(),
          component(nullptr)
    {}

    void ACamera::onLoad()
    {
        component = getScene()->createComponent<CCamera>();
        component->setOrigin(ObjectOrigin::Native);
        component->setId("Camera");
        component->attachTo(this);
    }
}
