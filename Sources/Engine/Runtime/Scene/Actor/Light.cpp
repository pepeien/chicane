#include "Chicane/Runtime/Scene/Actor/Light.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ALight::ALight()
        : Actor(),
          light(nullptr)
    {}

    void ALight::onLoad()
    {
        light = getScene()->createComponent<CLight>();
        light->setOrigin(ObjectOrigin::Native);
        light->setId("Light");
        light->attachTo(this);
        light->activate();
    }

    Component* ALight::getLight() const
    {
        return light;
    }
}
