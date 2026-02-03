#include "Chicane/Runtime/Scene/Component.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    Component::Component()
        : Transformable(),
          m_bCanTick(true),
          m_bIsActive(false),
          m_id(""),
          m_parent(nullptr),
          m_parentSubscription({})
    {}

    bool Component::isActive() const
    {
        return m_bIsActive;
    }

    void Component::activate()
    {
        if (!Application::getInstance().hasScene())
        {
            return;
        }

        m_bIsActive = true;

        onActivation();
    }

    void Component::deactivate()
    {
        if (!Application::getInstance().hasScene())
        {
            return;
        }

        m_bIsActive = false;

        onDeactivation();
    }

    bool Component::canTick() const
    {
        return m_bIsActive && m_bCanTick;
    }

    void Component::setCanTick(bool inCanTick)
    {
        m_bCanTick = inCanTick;
    }

    void Component::tick(float inDeltaTime)
    {
        if (!canTick())
        {
            return;
        }

        onTick(inDeltaTime);
    }

    const String& Component::getId() const
    {
        return m_id;
    }

    void Component::setId(const String& inId)
    {
        m_id = inId;
    }

    bool Component::isAttached() const
    {
        return m_parent != nullptr;
    }

    void Component::attachTo(Transformable* inParent)
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