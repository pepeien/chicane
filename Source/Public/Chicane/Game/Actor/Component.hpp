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

        bool hasOwner() const;
        template<class T = Actor>
        T* getOwner() const {
            return dynamic_cast<T*>(m_owner);
        }
        void setOwner(Actor* inActor);

    public:
        virtual const Vec<3, float>& getTranslation() const { return m_transform.translation; }
        virtual const Vec<3, float>& getRotation() const { return m_transform.rotation; }
        virtual const Vec<3, float>& getScale() const { return m_transform.scale; }
        virtual void setTranslation(const Vec<3, float>& inTranslation) { m_transform.translation = inTranslation; }
        virtual void setRotation(const Vec<3, float>& inRotation) { m_transform.rotation = inRotation; }
        virtual void setScale(const Vec<3, float>& inScale) { m_transform.scale = inScale; }

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