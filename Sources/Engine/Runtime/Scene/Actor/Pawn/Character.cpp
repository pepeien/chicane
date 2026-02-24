#include "Chicane/Runtime/Scene/Actor/Pawn/Character.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    ACharacter::ACharacter()
        : APawn()
    {}

    void ACharacter::move(const Vec3& inDirection, float inScale)
    {
        m_physics->moveTo(getTranslation() + (inDirection * inScale));
    }

    void ACharacter::jump()
    {
        m_physics->addImpulse(getUp(), 10.0f, getBottom());
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