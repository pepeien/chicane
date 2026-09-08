#pragma once

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Collision/Preset.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

namespace Chicane
{
    namespace Kerb
    {
        struct CHICANE_KERB CollisionPresetInfo
        {
        public:
            ObjectLayer layer     = ObjectLayer::Auto;
            bool        bIsSensor = false;
        };

        CHICANE_KERB CollisionPresetInfo resolveCollisionPreset(CollisionPreset inPreset, MotionType inMotion);
    }
}
