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

    void ALight::onLoad()
    {
        if (!light)
        {
            for (Object* attachment : getAttachments())
            {
                if (CLight* existing = dynamic_cast<CLight*>(attachment))
                {
                    light = existing;

                    break;
                }
            }
        }

        if (!light)
        {
            light = getScene()->createComponent<CLight>();
            light->attachTo(this);
        }

        light->activate();
    }
}
