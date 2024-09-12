#include "Chicane/Game/Actor/Component.hpp"

namespace Chicane
{
    ActorComponent::ActorComponent()
        : Transformable(),
        m_canTick(true),
        m_isActive(false),
        m_base({}),
        m_attachment(nullptr),
        m_attachmentTransformSubscription(nullptr)  
    {}

    bool ActorComponent::isActive() const
    {
        return m_isActive;
    }

    void ActorComponent::activate()
    {
        m_isActive = true;

        onActivation();
    }

    void ActorComponent::deactivate()
    {
        m_isActive = false;

        onDeactivation();
    }

    bool ActorComponent::canTick() const
    {
        return m_isActive && m_canTick;
    }

    void ActorComponent::setCanTick(bool inCanTick)
    {
        m_canTick = inCanTick;
    }

    void ActorComponent::tick(float inDeltaTime)
    {
        if (!canTick())
        {
            return;
        }

        onTick(inDeltaTime);
    }

    bool ActorComponent::isAttached() const
    {
        return m_attachment != nullptr;
    }

    void ActorComponent::attachTo(Actor* inAttachment)
    {
        m_attachment = inAttachment;

        if (isAttached())
        {  
            if (m_attachmentTransformSubscription)
            {
                m_attachmentTransformSubscription->complete();
            }

            m_attachmentTransformSubscription = m_attachment->watchTransform(
                [&](const Transform& inTransform)
                {
                    refreshTransform();
                }
            );
        }

        onAttachment(inAttachment);
    }

    const Transform& ActorComponent::getBase() const
    {
        return m_base;
    }

    void ActorComponent::setBase(const Transform& inOrigin)
    {
        m_base = inOrigin;

        refreshTransform();
    }

    const Vec<3, float>& ActorComponent::getBaseScale() const
    {
        return m_base.scale;
    }

    void ActorComponent::setBaseScale(const Vec<3, float>& inScale)
    {
        m_base.scale = inScale;

        refreshTransform();
    }

    const Vec<3, float>& ActorComponent::getBaseRotation() const
    {
        return m_base.rotation;
    }

    void ActorComponent::setBaseRotation(const Vec<3, float>& inRotation)
    {
        m_base.rotation = inRotation;

        refreshTransform();
    }

    const Vec<3, float>& ActorComponent::getBaseTranslation() const
    {
        return m_base.translation;
    }

    void ActorComponent::setBaseTranslation(const Vec<3, float>& inTranslation)
    {
        m_base.translation = inTranslation;

        refreshTransform();
    }

    void ActorComponent::refreshTransform()
    {
        if (!isAttached())
        {
            setAbsoluteScale(m_base.scale);
            setAbsoluteRotation(m_base.rotation);
            setAbsoluteTranslation(m_base.translation);

            return;
        }

        const Transform& attachmentTransform = m_attachment->getTransform();

        setAbsoluteScale(attachmentTransform.scale * m_base.scale);
        setAbsoluteRotation(attachmentTransform.rotation + m_base.rotation);
        setAbsoluteTranslation(attachmentTransform.translation + m_base.translation);
    }
}