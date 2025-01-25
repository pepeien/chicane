#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Instance.hpp"

namespace Chicane
{   
    class Actor;

    class Component : public Transformable
    {
    public:
        Component();
        virtual ~Component() = default;

    protected:
        virtual void onActivation() { return; }
        virtual void onDeactivation() { return; }
        virtual void onAttachment(Actor* inActor) { return; }
        virtual void onTransform() { return; }
        virtual void onTick(float inDeltaTime) { return; }

    public:
        template<class T>
        bool isType() const
        {
            return typeid(*this) == typeid(T);
        }

        bool isActive() const;
        void activate();
        void deactivate();

        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        bool isAttached() const;
        template<class T = Actor>
        T* getAttachment() const {
            return dynamic_cast<T*>(m_attachment);
        }
        void attachTo(Actor* inAttachment);

    protected:
        bool                 m_bCanTick;
        bool                 m_bIsActive;

        Actor*               m_attachment;
        Subscription<void*>* m_attachmentTransformSubscription;
    };
}