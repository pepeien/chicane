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

        inline String toString(ObjectLayer inValue)
        {
            switch (inValue)
            {
            case ObjectLayer::NonMoving:
                return "NonMoving";

            case ObjectLayer::Moving:
                return "Moving";

            case ObjectLayer::Pawn:
                return "Pawn";

            case ObjectLayer::PhysicsBody:
                return "PhysicsBody";

            case ObjectLayer::Projectile:
                return "Projectile";

            case ObjectLayer::Trigger:
                return "Trigger";

            case ObjectLayer::Debris:
                return "Debris";

            case ObjectLayer::NoCollision:
                return "NoCollision";

            case ObjectLayer::Count:
                return "Count";

            case ObjectLayer::Auto:
                return "Auto";

            default:
                return "";
            }
        }

        CHICANE_KERB ObjectLayer resolveObjectLayer(ObjectLayer inLayer, MotionType inMotion);
    }
}
