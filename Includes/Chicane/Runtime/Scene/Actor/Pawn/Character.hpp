#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Core/Time.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    class CHICANE_RUNTIME ACharacter : public APawn
    {
    public:
        ACharacter();

    public:
        void setMoveScale(float inScale);
        void setMoveInput(float inForward, float inRight, float inUp = 0.0f);
        void move(const Vec3& inDirection, float inScale);
        void jump();

        void addPitch(float inValue);
        void addRoll(float inValue);
        void addYaw(float inValue);

    protected:
        void onInput() override;

    protected:
        float       m_moveScale;
        float       m_forwardInput;
        float       m_rightInput;
        float       m_upInput;
        Time::Point m_lastInputTime;
        bool        m_bMoving;
    };
}
