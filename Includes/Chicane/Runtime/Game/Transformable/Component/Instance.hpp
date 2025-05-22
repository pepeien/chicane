#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game/Transformable/Instance.hpp"

namespace Chicane
{   
    class CHICANE_RUNTIME Component : public Transformable
    {
    protected:
        using Super = Component;

    public:
        Component();
        virtual ~Component() = default;

    protected:
        virtual void onActivation() { return; }
        virtual void onDeactivation() { return; }
        virtual void onAttachment(Transformable* inRoot) { return; }
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
        template<class T = Transformable>
        T* getAttachment() const {
            return static_cast<T*>(m_attachment);
        }
        void attachTo(Transformable* inRoot);

    public:
        bool                 m_bCanTick;
        bool                 m_bIsActive;

        Transformable*       m_attachment;
        Subscription<void*>* m_attachmentTransformSubscription;
    };
}