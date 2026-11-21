#include "Chicane/Kerb/Object/Layer.hpp"
#include "Chicane/Kerb/Object/Layer.reflected.hpp"

#include <string>

namespace Chicane
{
    namespace Kerb
    {
        static constexpr std::uint8_t LAYER_COUNT = static_cast<std::uint8_t>(ObjectLayer::Count);

        static bool g_customPairs[LAYER_COUNT][LAYER_COUNT] = {};
        static std::function<void(ObjectLayer, ObjectLayer, bool)> g_pairCallback = {};

        ObjectLayer resolveObjectLayer(ObjectLayer inLayer, MotionType inMotion)
        {
            if (inLayer != ObjectLayer::Auto)
            {
                return inLayer;
            }

            return inMotion == MotionType::Static ? ObjectLayer::NonMoving : ObjectLayer::Moving;
        }

        bool canCollide(ObjectLayer inA, ObjectLayer inB)
        {
            const auto rank = [](ObjectLayer inLayer)
            {
                return static_cast<std::uint8_t>(inLayer);
            };

            if (rank(inA) >= rank(ObjectLayer::Count) || rank(inB) >= rank(ObjectLayer::Count) ||
                inA == ObjectLayer::NoCollision || inB == ObjectLayer::NoCollision)
            {
                return false;
            }

            if (isCustomLayer(inA) || isCustomLayer(inB))
            {
                return g_customPairs[rank(inA)][rank(inB)];
            }

            if (rank(inA) > rank(inB))
            {
                const ObjectLayer swap = inA;
                inA                    = inB;
                inB                    = swap;
            }

            switch (inA)
            {
            case ObjectLayer::NonMoving:
                return inB != ObjectLayer::NonMoving;

            case ObjectLayer::Moving:
                return true;

            case ObjectLayer::Pawn:
                return inB != ObjectLayer::Debris;

            case ObjectLayer::PhysicsBody:
                return true;

            case ObjectLayer::Projectile:
                return inB == ObjectLayer::Trigger || inB == ObjectLayer::Debris;

            case ObjectLayer::Trigger:
                return inB == ObjectLayer::Debris;

            default:
                return false;
            }
        }

        void clearCustomLayerPairs(ObjectLayer inLayer)
        {
            if (!isCustomLayer(inLayer))
            {
                return;
            }

            for (std::uint8_t other = 0; other < LAYER_COUNT; ++other)
            {
                setCustomLayerPair(inLayer, static_cast<ObjectLayer>(other), false);
            }
        }

        void setCustomLayerPair(ObjectLayer inLayer, ObjectLayer inOther, bool bInCollide)
        {
            const auto rank = [](ObjectLayer inValue)
            {
                return static_cast<std::uint8_t>(inValue);
            };

            if (!isCustomLayer(inLayer) || rank(inOther) >= LAYER_COUNT || inOther == ObjectLayer::NoCollision)
            {
                return;
            }

            const std::uint8_t a = rank(inLayer);
            const std::uint8_t b = rank(inOther);
            if (g_customPairs[a][b] == bInCollide)
            {
                return;
            }

            g_customPairs[a][b] = bInCollide;
            g_customPairs[b][a] = bInCollide;

            if (g_pairCallback)
            {
                g_pairCallback(inLayer, inOther, bInCollide);
            }
        }

        void setObjectLayerPairCallback(std::function<void(ObjectLayer, ObjectLayer, bool)> inCallback)
        {
            g_pairCallback = std::move(inCallback);
        }
    }
}

namespace Chicane
{
    String toString(Kerb::ObjectLayer inValue)
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

        default:
            if (Kerb::isCustomLayer(inValue))
            {
                const int index = static_cast<int>(inValue) - static_cast<int>(Kerb::ObjectLayer::Custom_0);

                return String("Custom_") + String(std::to_string(index));
            }

            break;
        }

        switch (inValue)
        {
        case Kerb::ObjectLayer::Count:
            return "Count";

        case Kerb::ObjectLayer::Auto:
            return "Auto";

        default:
            return "";
        }
    }
}
