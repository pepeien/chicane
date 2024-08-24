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
        bool isActive() const;
        void activate();
        void deactivate();

        bool canTick() const;
        void setCanTick(bool inCanTick);

        const Vec<3, float>& getTranslation() const;
        void setTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getRotation() const;
        void setRotation(const Vec<3, float>& inRotation);

        bool hasOwner() const;
        template<class T = Actor>
        T* getOwner() const {
            return dynamic_cast<T*>(m_owner);
        }
        void setOwner(Actor* inActor);

    public:
        virtual void onActivation() { return; }
        virtual void onDeactivation() { return; }
        virtual void onAttachment() { return; }
        virtual void onTick(float inDeltaTime) { return; }

    protected:
        bool m_isActive;
        bool m_canTick;

        Transform m_transform;

        Actor* m_owner;
    };
}