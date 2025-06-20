#include "Chicane/Runtime/Scene/Component.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    Component::Component()
        : Transformable(),
        m_bCanTick(true),
        m_bIsActive(false),
        m_attachment(nullptr),
        m_attachmentTransformSubscription({})
    {}

    bool Component::isActive() const
    {
        return m_bIsActive;
    }

    void Component::activate()
    {
        if (!Application::hasScene())
        {
            return;
        }

        m_bIsActive = true;

        onActivation();
    }

    void Component::deactivate()
    {
        if (!Application::hasScene())
        {
            return;
        }

        m_bIsActive = false;

        onDeactivation();

        Application::getScene()->removeComponent(this);
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

    bool Component::isAttached() const
    {
        return m_attachment != nullptr;
    }

    void Component::attachTo(Transformable* inRoot)
    {
        if (inRoot == this)
        {
            return;
        }

        m_attachment = inRoot;

        if (isAttached())
        {  
            m_attachmentTransformSubscription.complete();
            m_attachmentTransformSubscription = m_attachment->watchChanges(
                [&]()
                {
                    Transform transform = {};
                    transform.setTranslation(m_attachment->getTranslation());
                    transform.setRotation(m_attachment->getRotation());
                    transform.setScale(m_attachment->getScale());

                    setAbsolute(transform);

                    onTransform();
                }
            );
        }

        onAttachment(inRoot);
    }
}