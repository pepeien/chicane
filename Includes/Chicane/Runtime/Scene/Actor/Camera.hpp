#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME ACamera : public Actor
    {
    public:
        CH_CONSTRUCTOR()
        ACamera();

    protected:
        void onLoad() override;
        void onPropertyEdited(const String& inName) override;

    public:
        void activate();
        void deactivate();

    public:
        CH_FIELD()
        String lookAt;

    protected:
        CCamera* m_camera;
    };
}