#include "Chicane/Runtime/Scene/Actor/Light.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ALight::ALight()
        : Actor(),
          m_light(nullptr)
    {}

    void ALight::onLoad()
    {
        m_light = getScene()->createComponent<CLight>();
        m_light->attachTo(this);
        m_light->activate();
    }
}