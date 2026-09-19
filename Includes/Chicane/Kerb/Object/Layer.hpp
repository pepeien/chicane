#pragma once

#include <cstdint>

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"
#include "Chicane/Core/String.hpp"

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

    inline String toString(Kerb::ObjectLayer inValue)
    {
        switch (inValue)
        {
        case Kerb::ObjectLayer::NonMoving:
            return "NonMoving";

        case Kerb::ObjectLayer::Moving:
            return "Moving";

        case Kerb::ObjectLayer::Pawn:
            return "Pawn";

        case Kerb::ObjectLayer::PhysicsBody:
            return "PhysicsBody";

        case Kerb::ObjectLayer::Projectile:
            return "Projectile";

        case Kerb::ObjectLayer::Trigger:
            return "Trigger";

        case Kerb::ObjectLayer::Debris:
            return "Debris";

        case Kerb::ObjectLayer::NoCollision:
            return "NoCollision";

        case Kerb::ObjectLayer::Count:
            return "Count";

        case Kerb::ObjectLayer::Auto:
            return "Auto";

        default:
            return "";
        }
    }
}
