#pragma once

#include "Core.hpp"
#include "Runtime/Game/Transformable/Actor/Pawn/Instance.hpp"
#include "Runtime/Game/Transformable/Component/Camera.hpp"

namespace Chicane
{
    class CHICANE ACharacter : public APawn
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