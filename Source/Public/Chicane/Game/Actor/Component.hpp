#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Game/Transformable.hpp"

namespace Chicane
{   
    class ActorComponent : public Transformable
    {
    public:
        ActorComponent();
        virtual ~ActorComponent() = default;

    protected:
        virtual void onActivation() { return; }
        virtual void onDeactivation() { return; }
        virtual void onAttachment(Actor* inActor) { return; }
        virtual void onTick(float inDeltaTime) { return; }

    public:
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

        const Transform& getBase() const;
        void setBase(const Transform& inOrigin);

        const Vec<3, float>& getBaseScale() const;
        void setBaseScale(const Vec<3, float>& inScale);

        const Vec<3, float>& getBaseRotation() const;
        void setBaseRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getBaseTranslation() const;
        void setBaseTranslation(const Vec<3, float>& inTranslation);

    private:
        void refreshTransform();

    protected:
        bool m_canTick;
        bool m_isActive;

        Transform m_base;

        Actor* m_attachment;
        Subscription<const Transform&>* m_attachmentTransformSubscription;
    };
}