#include "Chicane/Game/Transformable/Actor/Pawn/Character.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    ACharacter::ACharacter()
        : APawn()
    {
        setCanCollide(true);
    }

    void ACharacter::move(const Vec<3, float>& inDirection, float inScale)
    {
        addAbsoluteTranslation(inDirection * inScale);
    }

    void ACharacter::jump()
    {
        addForce(getUp(), 1.0f);
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