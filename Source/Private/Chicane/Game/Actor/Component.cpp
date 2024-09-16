#include "Chicane/Game/Actor/Component.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    ActorComponent::ActorComponent()
        : Transformable(),
        m_bCanTick(true),
        m_bIsActive(false),
        m_base({}),
        m_attachment(nullptr),
        m_attachmentTransformSubscription(nullptr)  
    {}

    bool ActorComponent::isActive() const
    {
        return m_bIsActive;
    }

    void ActorComponent::activate()
    {
        m_bIsActive = true;

        onActivation();
    }

    void ActorComponent::deactivate()
    {
        m_bIsActive = false;

        onDeactivation();
    }

    bool ActorComponent::canTick() const
    {
        return m_bIsActive && m_bCanTick;
    }

    void ActorComponent::setCanTick(bool inCanTick)
    {
        m_bCanTick = inCanTick;
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

        refreshTransform();

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
        Vec<3, float> scale       = m_base.scale;
        Vec<3, float> rotation    = m_base.rotation;
        Vec<3, float> translation = m_base.translation;

        if (isAttached())
        {
            const Transform& attachmentTransform = m_attachment->getTransform();

            scale       = scale * attachmentTransform.scale;
            rotation    = rotation + attachmentTransform.rotation;
            translation = translation + attachmentTransform.translation;
        }

        setAbsoluteScale(      scale);
        setAbsoluteRotation(   rotation);
        setAbsoluteTranslation(translation);
    }
}