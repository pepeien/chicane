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
