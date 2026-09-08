#include "Chicane/Runtime/Scene/Actor/Pawn/Character.reflected.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
    constexpr inline float MOVE_STEP_SECONDS = 0.016f;

    ACharacter::ACharacter()
        : APawn(),
          m_moveScale(3.0f),
          m_forwardInput(0.0f),
          m_rightInput(0.0f),
          m_upInput(0.0f),
          m_lastInputTime(Time::Clock::now()),
          m_bMoving(false)
    {}

    void ACharacter::setMoveScale(float inScale)
    {
        m_moveScale = std::max(0.0f, inScale);
    }

    void ACharacter::setMoveInput(float inForward, float inRight, float inUp)
    {
        m_forwardInput = inForward;
        m_rightInput   = inRight;
        m_upInput      = inUp;
    }

    void ACharacter::move(const Vec3& inDirection, float inScale)
    {
        const float dirLength = std::sqrt(inDirection.dot(inDirection));
        if (dirLength <= FLT_EPSILON)
        {
            return;
        }

        const Vec3 offset = (inDirection / dirLength) * inScale;
        m_forwardInput += offset.dot(getForward());
        m_rightInput += offset.dot(getRight());
        m_upInput += offset.dot(getUp());
    }

    void ACharacter::onInput()
    {
        Vec3 dir = getForward() * m_forwardInput + getRight() * m_rightInput + getUp() * m_upInput;

        const float length = std::sqrt(dir.dot(dir));
        if (length > 1.0f)
        {
            dir /= length;
        }

        const bool  bHasMove = std::sqrt(dir.dot(dir)) > FLT_EPSILON;
        const float speed    = m_moveScale / MOVE_STEP_SECONDS;

        if (!m_physics || !m_physics->isActive())
        {
            if (!bHasMove)
            {
                m_lastInputTime = Time::Clock::now();

                return;
            }

            const Time::Point now = Time::Clock::now();
            float             dt  = Time::seconds(now - m_lastInputTime);
            m_lastInputTime       = now;
            if (dt <= 0.0f || dt > 0.25f)
            {
                dt = MOVE_STEP_SECONDS;
            }

            addAbsoluteTranslation(dir * speed * dt);

            return;
        }

        if (!bHasMove)
        {
            m_physics->setHorizontalVelocity(Vec3::Zero());
            m_bMoving = false;

            return;
        }

        m_physics->setHorizontalVelocity(Vec3(dir.x, dir.y, 0.0f) * speed);
        m_bMoving = true;
    }

    void ACharacter::jump(float inSpeed)
    {
        const float speed = std::max(0.0f, inSpeed);

        if (!m_physics || !m_physics->isActive())
        {
            addAbsoluteTranslation(getUp() * speed);

            return;
        }

        Vec3 velocity = m_physics->getLinearVelocity();
        velocity.z    = speed;

        m_physics->setLinearVelocity(velocity);
    }

    void ACharacter::addPitch(float inValue)
    {
        addAbsoluteRotation(inValue, 0.0f, 0.0f);
    }

    void ACharacter::addRoll(float inValue)
    {
        addAbsoluteRotation(0.0f, inValue, 0.0f);
    }

    void ACharacter::addYaw(float inValue)
    {
        addAbsoluteRotation(0.0f, 0.0f, inValue);
    }
}
