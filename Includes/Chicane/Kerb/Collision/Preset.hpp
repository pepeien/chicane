#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        CH_ENUM()
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

    CHICANE_KERB String toString(Kerb::CollisionPreset inValue);
}
