#include "Chicane/Core/Math/Transform/Combined.hpp"

namespace Chicane
{
    namespace Transform
    {
        void Combined::combineTransforms()
        {
            translation = world.translation + relative.translation;
            rotation    = world.rotation + relative.rotation;
            scale       = world.scale * relative.scale;
        }
    }
}