#pragma once

#include "Base.hpp"

#include "Game/Actor.hpp"

namespace Chicane
{
    class ActorComponent
    {
    public:
        ActorComponent()
        :   m_owner(nullptr)
        {}
        virtual ~ActorComponent() = default;

    protected:
        bool hasOwner() { return m_owner != nullptr; }
        void setOwner(Actor* inActor) { m_owner = inActor; }
        template<class T = Actor>
        T* getOwner() { return dynamic_cast<T*>(m_owner); }

    protected:
        Actor* m_owner;
    };
}