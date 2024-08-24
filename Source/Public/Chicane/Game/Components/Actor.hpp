#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    class ActorComponent
    {
    public:
        ActorComponent();
        virtual ~ActorComponent() = default;

    public:
        bool canTick() const;
        void setCanTick(bool inCanTick);

        bool hasOwner() const;
        template<class T = Actor>
        T* getOwner() const {
            return dynamic_cast<T*>(m_owner);
        }
        void setOwner(Actor* inActor);

    public:
        virtual void activate() { return; }

        virtual void onAttachment() { return; }
        virtual void onTick(float inDeltaTime) { return; }

    protected:
        bool m_canTick;

        Actor* m_owner;
    };
}