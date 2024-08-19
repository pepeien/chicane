#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Camera.hpp"

namespace Chicane
{
    class CameraComponent : public Camera
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;
    };
}