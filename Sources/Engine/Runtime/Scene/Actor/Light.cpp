#include "Chicane/Runtime/Scene/Actor/Light.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    ALight::ALight()
        : Actor(),
          m_light(nullptr)
    {
        m_light = Application::getInstance().getScene()->createComponent<CLight>();
        m_light->attachTo(this);
        m_light->activate();
    }
}