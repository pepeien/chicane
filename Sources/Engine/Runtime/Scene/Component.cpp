#include "Chicane/Runtime/Scene/Component.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    Component::Component()
        : Object(),
          m_bIsActive(false),
          m_parent(nullptr),
          m_parentSubscription({})
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

    bool Component::isAttached() const
    {
        return m_parent != nullptr;
    }

    void Component::attachTo(Object* inParent)
    {
        if (!inParent || inParent == this)
        {
            return;
        }

        if (isAttached())
        {
            detach();
        }

        m_parent = inParent;

        m_parentSubscription = m_parent->watchChanges(
            [this]()
            {
                setAbsoluteTranslation(m_parent->getTranslation());
                setAbsoluteRotation(m_parent->getRotation());
                setAbsoluteScale(m_parent->getScale());
            }
        );

        onAttachment(inParent);
    }

    void Component::detach()
    {
        if (!isAttached())
        {
            return;
        }

        m_parentSubscription.complete();

        m_parent = nullptr;
    }
}