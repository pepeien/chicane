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

    protected:
        bool hasOwner() const;
        void setOwner(Actor* inActor);

        template<class T = Actor>
        T* getOwner() const {
            return dynamic_cast<T*>(m_owner);
        }

    protected:
        Actor* m_owner;
    };
}