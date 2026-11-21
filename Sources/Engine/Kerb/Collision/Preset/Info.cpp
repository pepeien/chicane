#include "Chicane/Kerb/Collision/Preset/Info.hpp"

namespace Chicane
{
    namespace Kerb
    {
        static constexpr std::uint8_t CUSTOM_PRESET_COUNT = 11;

        struct CustomPresetDefinition
        {
        public:
            bool bDefined  = false;
            bool bIsSensor = false;
        };

        static CustomPresetDefinition g_customPresets[CUSTOM_PRESET_COUNT] = {};

        static std::uint8_t customIndex(CollisionPreset inPreset)
        {
            return static_cast<std::uint8_t>(
                static_cast<std::uint8_t>(inPreset) - static_cast<std::uint8_t>(CollisionPreset::Custom_0)
            );
        }

        static ObjectLayer customLayer(CollisionPreset inPreset)
        {
            return static_cast<ObjectLayer>(
                static_cast<std::uint8_t>(ObjectLayer::Custom_0) + customIndex(inPreset)
            );
        }

        bool hasCustomPreset(CollisionPreset inPreset)
        {
            if (!isCustomPreset(inPreset))
            {
                return false;
            }

            return g_customPresets[customIndex(inPreset)].bDefined;
        }

        void defineCollisionPreset(
            CollisionPreset inPreset, bool bInSensor, const std::vector<ObjectLayer>& inLayers
        )
        {
            if (!isCustomPreset(inPreset))
            {
                return;
            }

            const std::uint8_t index = customIndex(inPreset);
            const ObjectLayer  layer = customLayer(inPreset);

            clearCustomLayerPairs(layer);

            g_customPresets[index].bDefined  = true;
            g_customPresets[index].bIsSensor = bInSensor;

            for (const ObjectLayer other : inLayers)
            {
                setCustomLayerPair(layer, other, true);
            }
        }

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

            default:
                if (isCustomPreset(inPreset) && hasCustomPreset(inPreset))
                {
                    info.layer     = customLayer(inPreset);
                    info.bIsSensor = g_customPresets[customIndex(inPreset)].bIsSensor;

                    break;
                }

                info.layer = ObjectLayer::Auto;

                break;
            }

            return info;
        }
    }
}
