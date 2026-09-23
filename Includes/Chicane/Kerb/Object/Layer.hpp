#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"

namespace Chicane
{
    namespace Kerb
    {
        CH_ENUM()
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
            Auto = 255 // Resolve at spawn time
        };

        CHICANE_KERB ObjectLayer resolveObjectLayer(ObjectLayer inLayer, MotionType inMotion);
    }

    CHICANE_KERB String toString(Kerb::ObjectLayer inValue);
}
