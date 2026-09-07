#include "Chicane/Grid/Style/Animation.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleAnimation::StyleAnimation()
            : name(""),
              duration(0.0f),
              delay(0.0f),
              iterations(1),
              easing(Drift::EasingCurve::ease()),
              bIsReverse(false),
              bIsAlternate(false),
              bIsPaused(false),
              bShouldFillForwards(false),
              bShouldFillBackwards(false)
        {}
    }
}
