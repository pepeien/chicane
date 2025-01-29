#include "Chicane/Game/Transformable/Actor/Pawn/Character.hpp"

constexpr float MOVEMENT_COEFFICIENT = 1.5f;

namespace Chicane
{
    ACharacter::ACharacter()
        : APawn()
    {}

    void ACharacter::move(const Vec<3, float>& inDirection, float inScale)
    {
        addAbsoluteTranslation(inDirection * inScale);
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