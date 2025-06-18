#include "Chicane/Runtime/Scene/Actor/Pawn/Character.hpp"

namespace Chicane
{
    ACharacter::ACharacter()
        : APawn()
    {}

    void ACharacter::move(const Vec3& inDirection, float inScale)
    {
        addAbsoluteTranslation(inDirection * inScale);
    }

    void ACharacter::jump()
    {
        getPhysicsComponent()->addForce(getUp(), 0.22f);
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