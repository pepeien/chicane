#pragma once

#include <cstdint>

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
}
