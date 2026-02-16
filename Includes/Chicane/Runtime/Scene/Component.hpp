#pragma once

#include "Chicane/Core/Event/Subscription.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Component : public Object
    {
    public:
        Component();

    protected:
        inline virtual void onActivation() { return; }
        inline virtual void onDeactivation() { return; }
        inline virtual void onAttachment(Object* inParent) { return; }

    public:
        template <class T>
        inline bool isType() const
        {
            return typeid(*this) == typeid(T);
        }

        bool isActive() const;
        void activate();
        void deactivate();

        bool isAttached() const;
        template <class T = Object>
        inline T* getParent() const
        {
            return static_cast<T*>(m_parent);
        }
        void attachTo(Object* inParent);
        void detach();

    public:
        bool                m_bIsActive;

        Object*             m_parent;
        EventSubscription<> m_parentSubscription;
    };
}