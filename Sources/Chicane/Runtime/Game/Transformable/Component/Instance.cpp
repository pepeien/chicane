#include "Chicane/Runtime/Game/Transformable/Component/Instance.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    Component::Component()
        : Transformable(),
        m_bCanTick(true),
        m_bIsActive(false),
        m_attachment(nullptr),
        m_attachmentTransformSubscription(nullptr)  
    {}

    bool Component::isActive() const
    {
        return m_bIsActive;
    }

    void Component::activate()
    {
        if (!Application::hasLevel())
        {
            return;
        }

        m_bIsActive = true;

        onActivation();
    }

    void Component::deactivate()
    {
        if (!Application::hasLevel())
        {
            return;
        }

        m_bIsActive = false;

        onDeactivation();

        Application::getLevel()->removeComponent(this);
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
            if (m_attachmentTransformSubscription)
            {
                m_attachmentTransformSubscription->complete();
            }

            m_attachmentTransformSubscription = m_attachment->watchTransform(
                [&](void* inData)
                {
                    Transform::Instance transform = {};
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