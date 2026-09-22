#include "Chicane/Kerb/Object/Layer.hpp"

namespace Chicane
{
    namespace Kerb
    {
        ObjectLayer resolveObjectLayer(ObjectLayer inLayer, MotionType inMotion)
        {
            if (inLayer != ObjectLayer::Auto)
            {
                return inLayer;
            }

            return inMotion == MotionType::Static ? ObjectLayer::NonMoving : ObjectLayer::Moving;
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

        case Kerb::ObjectLayer::Count:
            return "Count";

        case Kerb::ObjectLayer::Auto:
            return "Auto";

        default:
            return "";
        }
    }
}
