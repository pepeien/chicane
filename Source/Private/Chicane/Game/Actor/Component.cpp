#include "Chicane/Game/Actor/Component.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    ActorComponent::ActorComponent()
        : m_canTick(true),
        m_isActive(false),
        m_attachmentRule(AttachmentRule::FollowAll),
        m_owner(nullptr)
    {
        addComponent(this);

        watchTransform(
            [this](const Transform& inTransform)
            {
                refreshTranslation();
                refreshRotation();
                refreshScale();
            }
        );
    }

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

    AttachmentRule ActorComponent::getAttachmentRule() const
    {
        return m_attachmentRule;
    }

    void ActorComponent::setAttachmentRule(AttachmentRule inRule)
    {
        m_attachmentRule = inRule;
    }

    bool ActorComponent::hasOwner() const
    {
        return m_owner != nullptr;
    }

    void ActorComponent::setOwner(Actor* inActor)
    {
        m_owner = inActor;

        if (inActor)
        {
            m_owner->watchTransform(
                [this](const Transform& inTransform)
                {
                    refreshTranslation();
                    refreshRotation();
                    refreshScale();
                }
            );
        }

        onAttachment();
    }

    const Vec<3, float>& ActorComponent::getOriginTranslation() const
    {
        return m_origin.translation;
    }

    void ActorComponent::setOriginTranslation(const Vec<3, float>& inTranslation)
    {
        bool isIdentical = std::fabs(m_origin.translation.x - inTranslation.x) < FLT_EPSILON &&
                           std::fabs(m_origin.translation.y - inTranslation.y) < FLT_EPSILON &&
                           std::fabs(m_origin.translation.z - inTranslation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_origin.translation = inTranslation;
    }

    const Vec<3, float>& ActorComponent::getOriginRotation() const
    {
        return m_origin.rotation;
    }

    void ActorComponent::setOriginRotation(const Vec<3, float>& inRotation)
    {
        bool isIdentical = std::fabs(m_origin.rotation.x - inRotation.x) < FLT_EPSILON &&
                           std::fabs(m_origin.rotation.y - inRotation.y) < FLT_EPSILON &&
                           std::fabs(m_origin.rotation.z - inRotation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_origin.rotation = inRotation;
    }

    const Vec<3, float>& ActorComponent::getOriginScale() const
    {
        return m_origin.scale;
    }

    void ActorComponent::setOriginScale(const Vec<3, float>& inScale)
    {
        bool isIdentical = std::fabs(m_origin.rotation.x - inScale.x) < FLT_EPSILON &&
                           std::fabs(m_origin.rotation.y - inScale.y) < FLT_EPSILON &&
                           std::fabs(m_origin.rotation.z - inScale.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_origin.scale = inScale;
    }

    void ActorComponent::refreshTranslation()
    {
        if (m_attachmentRule != AttachmentRule::FollowAll && m_attachmentRule != AttachmentRule::FollowTranslation)
        {
            setAbsoluteTranslation(m_origin.translation);

            return;
        }

        if (!hasOwner())
        {
            return;
        }

        setAbsoluteTranslation(m_owner->getTranslation() + m_origin.translation);
    }

    void ActorComponent::refreshRotation()
    {
        if (m_attachmentRule != AttachmentRule::FollowAll && m_attachmentRule != AttachmentRule::FollowRotation)
        {
            setAbsoluteRotation(m_origin.rotation);

            return;
        }

        if (!hasOwner())
        {
            return;
        }

        setAbsoluteRotation(m_owner->getRotation() + m_origin.rotation);
    }

    void ActorComponent::refreshScale()
    {
        if (m_attachmentRule != AttachmentRule::FollowAll)
        {
            setAbsoluteRotation(m_origin.scale);

            return;
        }

        if (!hasOwner())
        {
            return;
        }

        setAbsoluteScale(m_owner->getScale() * m_origin.scale);
    }
}