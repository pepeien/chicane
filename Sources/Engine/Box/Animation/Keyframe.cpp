#include "Chicane/Box/Animation/Keyframe.hpp"

namespace Chicane
{
    namespace Box
    {
        AnimationKeyframe::AnimationKeyframe()
            : time(Time::sZero()),
              transform({}),
              easing("")
        {}
    }
}
