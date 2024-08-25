#include "Chicane/Game/Actor/Component.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    ActorComponent::ActorComponent()
        : m_canTick(false),
        m_isActive(false),
        m_attachmentRule(AttachmentRule::Idependent),
        m_owner(nullptr)
    {
        addComponent(this);
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

        updateTranslation();
        updateRotation();

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

    const Vec<3, float>& ActorComponent::getTranslation() const
    {
        return m_transform.translation;
    }

    void ActorComponent::setTranslation(const Vec<3, float>& inTranslation)
    {
        bool isIdentical = std::fabs(m_transform.translation.x - inTranslation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.y - inTranslation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.z - inTranslation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_transform.translation = inTranslation;
    }

    const Vec<3, float>& ActorComponent::getRotation() const
    {
        return m_transform.rotation;
    }

    void ActorComponent::setRotation(const Vec<3, float>& inRotation)
    {
        bool isIdentical = std::fabs(m_transform.rotation.x - inRotation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.y - inRotation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.z - inRotation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_transform.rotation = inRotation;
    }

    const Vec<3, float>& ActorComponent::getScale() const
    {
        return m_transform.scale;
    }

    void ActorComponent::setScale(const Vec<3, float>& inScale)
    {
        bool isIdentical = std::fabs(m_transform.rotation.x - inScale.x) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.y - inScale.y) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.z - inScale.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_transform.scale = inScale;
    }

    void ActorComponent::updateTranslation()
    {
        if (m_attachmentRule != AttachmentRule::FollowAll && m_attachmentRule != AttachmentRule::FollowTranslation)
        {
            return;
        }

        if (!hasOwner())
        {
            return;
        }

        setTranslation(m_owner->getTranslation() + m_origin.translation);
    }

    void ActorComponent::updateRotation()
    {
        if (m_attachmentRule != AttachmentRule::FollowAll && m_attachmentRule != AttachmentRule::FollowRotation)
        {
            return;
        }

        if (!hasOwner())
        {
            return;
        }

        setRotation(m_owner->getRotation() + m_origin.rotation);
    }
}