#include "Chicane/Runtime/Scene/Actor/Light.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    ALight::ALight()
        : Super(),
          m_light(nullptr)
    {
        m_light = Application::getScene()->createComponent<CLight>();
        m_light->attachTo(this);
        m_light->activate();
    }
}