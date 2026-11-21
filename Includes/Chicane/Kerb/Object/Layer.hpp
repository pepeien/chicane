#pragma once

#include <cstdint>
#include <functional>

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

            Custom_0,
            Custom_1,
            Custom_2,
            Custom_3,
            Custom_4,
            Custom_5,
            Custom_6,
            Custom_7,
            Custom_8,
            Custom_9,
            Custom_10,

            Count,
            Auto = 255 // Resolve at spawn time
        };

        inline bool isCustomLayer(ObjectLayer inLayer)
        {
            return inLayer >= ObjectLayer::Custom_0 && inLayer < ObjectLayer::Count;
        }

        CHICANE_KERB ObjectLayer resolveObjectLayer(ObjectLayer inLayer, MotionType inMotion);
        CHICANE_KERB bool canCollide(ObjectLayer inA, ObjectLayer inB);
        CHICANE_KERB void clearCustomLayerPairs(ObjectLayer inLayer);
        CHICANE_KERB void setCustomLayerPair(ObjectLayer inLayer, ObjectLayer inOther, bool bInCollide);
        CHICANE_KERB void setObjectLayerPairCallback(
            std::function<void(ObjectLayer, ObjectLayer, bool)> inCallback
        );
    }

    CHICANE_KERB String toString(Kerb::ObjectLayer inValue);
}
