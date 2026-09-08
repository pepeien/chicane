#pragma once

#include <cstdint>

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"

namespace Chicane
{
    namespace Kerb
    {
        enum class ObjectLayer : std::uint8_t
        {
            NonMoving = 0, // WorldStatic
            Moving,        // WorldDynamic
            Pawn,
            PhysicsBody,
            Projectile,
            Trigger,
            Debris,
            NoCollision,

            Count,
            Auto = 255 // Resolve from motion / collision preset at create time
        };

        CHICANE_KERB ObjectLayer resolveObjectLayer(ObjectLayer inLayer, MotionType inMotion);
    }
}
