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
        // Attributes
        static constexpr inline const char* LOOK_AT_ATTRIBUTE_NAME = "lookAt";

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