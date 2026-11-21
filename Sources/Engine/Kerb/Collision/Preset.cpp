#include "Chicane/Kerb/Collision/Preset.hpp"
#include "Chicane/Kerb/Collision/Preset.reflected.hpp"

#include <string>

namespace Chicane
{
    String toString(Kerb::CollisionPreset inValue)
    {
        switch (inValue)
        {
        case Kerb::CollisionPreset::Default:
            return "Default";

        case Kerb::CollisionPreset::BlockAll:
            return "BlockAll";

        case Kerb::CollisionPreset::NoCollision:
            return "NoCollision";

        case Kerb::CollisionPreset::Pawn:
            return "Pawn";

        case Kerb::CollisionPreset::PhysicsActor:
            return "PhysicsActor";

        case Kerb::CollisionPreset::Projectile:
            return "Projectile";

        case Kerb::CollisionPreset::OverlapAll:
            return "OverlapAll";

        default:
            if (Kerb::isCustomPreset(inValue))
            {
                const int index = static_cast<int>(inValue) - static_cast<int>(Kerb::CollisionPreset::Custom_0);

                return String("Custom_") + String(std::to_string(index));
            }

            return "";
        }
    }
}
