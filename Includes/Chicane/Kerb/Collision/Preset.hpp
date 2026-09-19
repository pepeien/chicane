#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Kerb
    {
        enum class CollisionPreset : std::uint8_t
        {
            Custom = 0,
            Default,
            BlockAll,
            NoCollision,
            Pawn,
            PhysicsActor,
            Projectile,
            OverlapAll
        };
    }

    inline String toString(Kerb::CollisionPreset inValue)
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
