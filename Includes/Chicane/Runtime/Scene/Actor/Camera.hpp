#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"

namespace Chicane
{
    CH_TYPE(Manual, Group = "Actor | Camera")
    class CHICANE_RUNTIME ACamera : public Actor
    {
    public:
        CH_CONSTRUCTOR()
        ACamera();

    protected:
        void onLoad() override;

    public:
        void activate();
        void deactivate();

    protected:
        CCamera* m_camera;
    };
}
