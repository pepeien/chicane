#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Game/Transformable.hpp"

namespace Chicane
{   
    enum class AttachmentRule : std::uint8_t
    {
        Idependent,
        FollowRotation,
        FollowTranslation,
        FollowAll
    };

    class ActorComponent : public Transformable
    {
    public:
        ActorComponent();
        virtual ~ActorComponent() = default;

    protected:
        virtual void onActivation() { return; }
        virtual void onDeactivation() { return; }
        virtual void onAttachment() { return; }
        virtual void onTick(float inDeltaTime) { return; }

    public:
        bool isActive() const;
        void activate();
        void deactivate();

        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        AttachmentRule getAttachmentRule() const;
        void setAttachmentRule(AttachmentRule inRule);

        bool hasOwner() const;
        template<class T = Actor>
        T* getOwner() const {
            return dynamic_cast<T*>(m_owner);
        }
        void setOwner(Actor* inActor);

        const Vec<3, float>& getOriginTranslation() const;
        void setOriginTranslation(const Vec<3, float>& inTranslation);

        const Vec<3, float>& getOriginRotation() const;
        void setOriginRotation(const Vec<3, float>& inRotation);

        const Vec<3, float>& getOriginScale() const;
        void setOriginScale(const Vec<3, float>& inScale);

    private:
        void refreshTranslation();
        void refreshRotation();
        void refreshScale();

    protected:
        bool m_canTick;
        bool m_isActive;

        AttachmentRule m_attachmentRule;

        Transform m_origin;

        Actor* m_owner;
    };
}