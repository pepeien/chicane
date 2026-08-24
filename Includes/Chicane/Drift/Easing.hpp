#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Easing/Type.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT EasingCurve
        {
        public:
            static EasingCurve linear();
            static EasingCurve ease();
            static EasingCurve easeIn();
            static EasingCurve easeOut();
            static EasingCurve easeInOut();
            static EasingCurve cubicBezier(float inX1, float inY1, float inX2, float inY2);
            static EasingCurve fromString(const String& inValue);

        public:
            EasingCurve();
            EasingCurve(Easing inType);
            EasingCurve(float inX1, float inY1, float inX2, float inY2);

        public:
            float evaluate(float inTime) const;

        private:
            float sampleX(float inT) const;
            float sampleY(float inT) const;
            float sampleDerivativeX(float inT) const;
            float solve(float inTime) const;

        public:
            Easing type;
            float  x1;
            float  y1;
            float  x2;
            float  y2;
        };
    }
}
