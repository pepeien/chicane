#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Pawn/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Camera.hpp"

namespace Chicane
{
    class ACharacter : public APawn
    {
    public:
        ACharacter();
        virtual ~ACharacter() = default;

    public:
        void move(const Vec<3, float>& inDirection, float inScale);

        void addPitch(float inValue);
        void addRoll(float inValue);
        void addYaw(float inValue);
    };
}