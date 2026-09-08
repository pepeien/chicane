#include "Chicane/Kerb/Collision/Preset/Info.hpp"

namespace Chicane
{
    namespace Kerb
    {
        CollisionPresetInfo resolveCollisionPreset(CollisionPreset inPreset, MotionType inMotion)
        {
            CollisionPresetInfo info;

            switch (inPreset)
            {
            case CollisionPreset::NoCollision:
                info.layer = ObjectLayer::NoCollision;

                break;

            case CollisionPreset::Pawn:
                info.layer = ObjectLayer::Pawn;

                break;

            case CollisionPreset::PhysicsActor:
                info.layer = ObjectLayer::PhysicsBody;

                break;

            case CollisionPreset::Projectile:
                info.layer = ObjectLayer::Projectile;

                break;

            case CollisionPreset::OverlapAll:
                info.layer     = ObjectLayer::Trigger;
                info.bIsSensor = true;

                break;

            case CollisionPreset::BlockAll:
            case CollisionPreset::Default:
                info.layer = resolveObjectLayer(ObjectLayer::Auto, inMotion);

                break;

            case CollisionPreset::Custom:
            default:
                info.layer = ObjectLayer::Auto;

                break;
            }

            return info;
        }
    }
}
