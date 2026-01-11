#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ACamera : public Actor
    {
    public:
        ACamera();
        virtual ~ACamera() = default;

    public:
        void activate();
        void deactivate();

    protected:
        CCamera* m_camera;
    };
}