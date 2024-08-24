#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Components/Actor.hpp"
#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    class CameraComponent : public ActorComponent
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;

    public:
        void activate() override;

        void onAttachment() override;
        void onTick(float inDeltaTime) override;

    public:
        bool willFollowOwner() const;
        void setWillFollowOwner(bool inWillFollowOwner);

    protected:
        void refreshPosition();

    protected:
        bool m_willFollowOwner;

        std::unique_ptr<Camera> m_camera;
    };
}