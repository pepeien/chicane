#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift/Easing/Curve.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleAnimation
        {
        public:
            StyleAnimation();

        public:
            String             name;
            float              duration;
            float              delay;
            int                iterations;
            Drift::EasingCurve easing;
            bool               bIsReverse;
            bool               bIsAlternate;
            bool               bIsPaused;
            bool               bShouldFillForwards;
            bool               bShouldFillBackwards;
        };
    }
}
