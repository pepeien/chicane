#pragma once

#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Transformable.hpp"

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
        virtual void onAttachment(Transformable* inParent) { return; }
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

        const String& getId() const;
        void setId(const String& inId);

        bool isAttached() const;
        template<class T = Transformable>
        T* getParent() const {
            return static_cast<T*>(m_parent);
        }
        void attachTo(Transformable* inParent);

    public:
        bool            m_bCanTick;
        bool            m_bIsActive;

        String          m_id;

        Transformable*  m_parent;
        Subscription<>  m_parentSubscription;
    };
}