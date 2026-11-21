#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

namespace Chicane
{
    namespace Kerb
    {
        CH_ENUM()
        enum class CollisionPreset : std::uint8_t
        {
            Default = 0,
            BlockAll,
            NoCollision,
            Pawn,
            PhysicsActor,
            Projectile,
            OverlapAll,

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
        };

        inline bool isCustomPreset(CollisionPreset inPreset)
        {
            return inPreset >= CollisionPreset::Custom_0 && inPreset <= CollisionPreset::Custom_10;
        }

        CHICANE_KERB bool hasCustomPreset(CollisionPreset inPreset);
        CHICANE_KERB void defineCollisionPreset(
            CollisionPreset                 inPreset,
            bool                            bInSensor,
            const std::vector<ObjectLayer>& inLayers
        );
    }

    CHICANE_KERB String toString(Kerb::CollisionPreset inValue);
}
