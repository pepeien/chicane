#include "Chicane/Game/Transformable/Actor/Light/Instance.hpp"

namespace Chicane
{
    Light::Light()
        : Actor(),
        m_light(new LightComponent())
    {
        m_light->attachTo(this);
        m_light->activate();
    }
}