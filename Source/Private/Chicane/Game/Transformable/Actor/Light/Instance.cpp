#include "Chicane/Game/Transformable/Actor/Light/Instance.hpp"

#include "Chicane/Application.hpp"

namespace Chicane
{
    ALight::ALight()
        : Actor(),
        m_light(nullptr)
    {
        m_light = Application::getLevel()->createComponent<CLight>();
        m_light->attachTo(this);
        m_light->activate();
    }
}