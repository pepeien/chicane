#include "Chicane/Runtime/Scene/Actor/Light.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ALight::ALight()
        : Actor(),
          light(nullptr)
    {}

    Component* ALight::getLight() const
    {
        return light;
    }

    void ALight::createDefaultComponents()
    {
        light = createDefaultComponent<CLight>("Light");
        if (light)
        {
            light->activate();
        }
    }
}
