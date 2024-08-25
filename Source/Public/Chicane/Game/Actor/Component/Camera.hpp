#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor/Component.hpp"
#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    class CameraComponent : public ActorComponent
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;

    public:
        void onActivation() override;
        void onDeactivation() override;
        void onTick(float inDeltaTime) override;

    protected:
        std::unique_ptr<Camera> m_camera;
    }; 
}