#include "Chicane/Kerb/Collision/Preset.hpp"

namespace Chicane
{
    String toString(Kerb::CollisionPreset inValue)
    {
        switch (inValue)
        {
        case Kerb::CollisionPreset::Custom:
            return "Custom";

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
            return "";
        }
    }
}
