#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game/Transformable/Actor/Pawn/Instance.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Camera.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ACharacter : public APawn
    {
    public:
        ACharacter();
        virtual ~ACharacter() = default;

    public:
        void move(const Vec<3, float>& inDirection, float inScale);
        void jump();

        void addPitch(float inValue);
        void addRoll(float inValue);
        void addYaw(float inValue);
    };
}