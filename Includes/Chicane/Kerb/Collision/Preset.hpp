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

        inline String toString(CollisionPreset inValue)
        {
            switch (inValue)
            {
            case CollisionPreset::Custom:
                return "Custom";

            case CollisionPreset::Default:
                return "Default";

            case CollisionPreset::BlockAll:
                return "BlockAll";

            case CollisionPreset::NoCollision:
                return "NoCollision";

            case CollisionPreset::Pawn:
                return "Pawn";

            case CollisionPreset::PhysicsActor:
                return "PhysicsActor";

            case CollisionPreset::Projectile:
                return "Projectile";

            case CollisionPreset::OverlapAll:
                return "OverlapAll";

            default:
                return "";
            }
        }
    }
}
