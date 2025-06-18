#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME ACharacter : public APawn
    {
    public:
        ACharacter();
        virtual ~ACharacter() = default;

    public:
        void move(const Vec3& inDirection, float inScale);
        void jump();

        void addPitch(float inValue);
        void addRoll(float inValue);
        void addYaw(float inValue);
    };
}