#include "Chicane/Runtime/Scene/Actor/Pawn/Character.hpp"

namespace Chicane
{
    ACharacter::ACharacter()
        : APawn()
    {}

    void ACharacter::move(const Vec3& inDirection, float inScale)
    {
        if (!m_physics->isActive())
        {
            addAbsoluteTranslation(inDirection * inScale);

            return;
        }

        m_physics->moveTo(getTranslation() + (inDirection * inScale));
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