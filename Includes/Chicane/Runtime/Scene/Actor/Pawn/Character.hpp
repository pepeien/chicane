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
        static constexpr inline const float DEFAULT_JUMP_SPEED = 12.0f;

    public:
        ACharacter();

    public:
        CH_FUNCTION()
        void setMoveScale(float inScale);

        CH_FUNCTION()
        void setMoveInput(float inForward, float inRight, float inUp = 0.0f);

        CH_FUNCTION()
        void move(const Vec3& inDirection, float inScale);

        CH_FUNCTION()
        void jump(float inSpeed = DEFAULT_JUMP_SPEED);

        CH_FUNCTION()
        void addPitch(float inValue);

        CH_FUNCTION()
        void addRoll(float inValue);

        CH_FUNCTION()
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
