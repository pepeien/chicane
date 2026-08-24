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
              bReverse(false),
              bAlternate(false),
              bPaused(false),
              bFillForwards(false),
              bFillBackwards(false)
        {}
    }
}
