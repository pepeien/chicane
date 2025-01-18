#include "Chicane/Game/Transformable/Component/Instance.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    Component::Component()
        : Transformable(),
        m_bCanTick(true),
        m_bIsActive(false),
        m_base({}),
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

        Application::getLevel()->addComponent(this);
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

    void Component::attachTo(Actor* inAttachment)
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
                    updateTransform();
                }
            );
        }

        updateTransform();

        onAttachment(inAttachment);
    }

    const Transform& Component::getBase() const
    {
        return m_base;
    }

    void Component::setBase(const Transform& inOrigin)
    {
        m_base = inOrigin;

        updateTransform();
    }

    const Vec<3, float>& Component::getBaseScale() const
    {
        return m_base.scale;
    }

    void Component::setBaseScale(const Vec<3, float>& inScale)
    {
        m_base.scale = inScale;

        updateTransform();
    }

    const Vec<3, float>& Component::getBaseRotation() const
    {
        return m_base.rotation;
    }

    void Component::setBaseRotation(const Vec<3, float>& inRotation)
    {
        m_base.rotation = inRotation;

        updateTransform();
    }

    const Vec<3, float>& Component::getBaseTranslation() const
    {
        return m_base.translation;
    }

    void Component::setBaseTranslation(const Vec<3, float>& inTranslation)
    {
        m_base.translation = inTranslation;

        updateTransform();
    }

    void Component::updateTransform()
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