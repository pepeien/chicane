#include "Chicane/Runtime/Scene/Actor/Pawn/Character.reflected.hpp"

#include <cfloat>
#include <cmath>

namespace Chicane
{
    static constexpr float MOVE_STEP_SECONDS     = 0.016f;
    static constexpr float MOVE_INPUT_TIMEOUT_MS = 32.0f;

    ACharacter::ACharacter()
        : APawn(),
          m_pendingMove(Vec3::Zero()),
          m_pendingStep(0.0f),
          m_desiredMove(Vec3::Zero()),
          m_lastMoveTime({}),
          m_bMoving(false)
    {}

    void ACharacter::move(const Vec3& inDirection, float inScale)
    {
        const float dirLength = std::sqrt(inDirection.dot(inDirection));
        if (dirLength <= FLT_EPSILON)
        {
            return;
        }

        const Vec3 offset = (inDirection / dirLength) * inScale;
        m_pendingMove += offset;

        const float step = std::abs(inScale);
        if (step > m_pendingStep)
        {
            m_pendingStep = step;
        }
    }

    void ACharacter::onInput()
    {
        Vec3        offset  = m_pendingMove;
        const float maxStep = m_pendingStep;

        m_pendingMove = Vec3::Zero();
        m_pendingStep = 0.0f;

        const float length = std::sqrt(offset.dot(offset));
        if (length > FLT_EPSILON)
        {
            if (maxStep > 0.0f && length > maxStep)
            {
                offset *= maxStep / length;
            }

            m_desiredMove  = offset;
            m_lastMoveTime = Time::Clock::now();
        }

        const bool bHasMove =
            Time::miliseconds(Time::Clock::now() - m_lastMoveTime) <= MOVE_INPUT_TIMEOUT_MS &&
            std::sqrt(m_desiredMove.dot(m_desiredMove)) > FLT_EPSILON;

        if (!m_physics || !m_physics->isActive())
        {
            if (length > FLT_EPSILON)
            {
                addAbsoluteTranslation(m_desiredMove);
            }

            return;
        }

        if (!bHasMove && !m_bMoving)
        {
            return;
        }

        if (bHasMove)
        {
            m_physics->setHorizontalVelocity(m_desiredMove / MOVE_STEP_SECONDS);
            m_bMoving = true;
        }
        else
        {
            m_physics->setHorizontalVelocity(Vec3::Zero());
            m_bMoving = false;
        }
    }

    void ACharacter::jump()
    {
        if (!m_physics->isActive())
        {
            addAbsoluteTranslation(getUp() * 1000.0f);

            return;
        }

        m_physics->addImpulse(getUp(), 1000.0f, getBottom());
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
