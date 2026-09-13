#include "Chicane/Smoke/Range.hpp"

namespace Chicane
{
    namespace Smoke
    {
        Range::Range()
            : from(0.0f),
              to(0.0f)
        {}

        Range::Range(float inValue)
            : from(inValue),
              to(inValue)
        {}

        Range::Range(float inFrom, float inTo)
            : from(inFrom),
              to(inTo)
        {}

        float Range::sample(float inAlpha) const
        {
            return from + (to - from) * inAlpha;
        }
    }
}
