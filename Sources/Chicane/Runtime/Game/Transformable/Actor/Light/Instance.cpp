#include "Chicane/Runtime/Game/Transformable/Actor/Light/Instance.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    ALight::ALight()
        : Super(),
        m_light(nullptr)
    {
        m_light = Application::getLevel()->createComponent<CLight>();
        m_light->attachTo(this);
        m_light->activate();
    }
}