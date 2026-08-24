#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift/Easing.hpp"

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
            bool               bReverse;
            bool               bAlternate;
            bool               bPaused;
            bool               bFillForwards;
            bool               bFillBackwards;
        };
    }
}
