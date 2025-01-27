#include "Chicane/Game/Transformable/Actor/Light/Instance.hpp"

namespace Chicane
{
    ALight::ALight()
        : Actor(),
        m_light(new CLight())
    {
        m_light->attachTo(this);
        m_light->activate();
    }
}