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
        void move(const Vec3& inDirection, float inScale);
        void jump();

        void addPitch(float inValue);
        void addRoll(float inValue);
        void addYaw(float inValue);

    protected:
        void onInput() override;

    protected:
        Vec3        m_pendingMove;
        float       m_pendingStep;
        Vec3        m_desiredMove;
        Time::Point m_lastMoveTime;
        bool        m_bMoving;
    };
}