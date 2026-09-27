#include "Chicane/Runtime/Scene/Component.reflected.hpp"

namespace Chicane
{
    Component::Component()
        : Object(),
          m_bIsActive(false)
    {}

    bool Component::isActive() const
    {
        return m_bIsActive;
    }

    void Component::activate()
    {
        m_bIsActive = true;

        onActivation();
    }

    void Component::deactivate()
    {
        m_bIsActive = false;

        onDeactivation();
    }
}