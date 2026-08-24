#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Easing.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT EasingCurve
        {
        public:
            static constexpr inline const char* TYPE_LINEAR          = "linear";
            static constexpr inline const char* TYPE_EASE            = "ease";
            static constexpr inline const char* TYPE_EASE_IN         = "ease-in";
            static constexpr inline const char* TYPE_EASE_OUT        = "ease-out";
            static constexpr inline const char* TYPE_EASE_IN_OUT     = "ease-in-out";
            static constexpr inline const char* CUBIC_BEZIER_KEYWORD = "cubic-bezier";

            static constexpr inline float       LINEAR_X1 = 0.0f;
            static constexpr inline float       LINEAR_Y1 = 0.0f;
            static constexpr inline float       LINEAR_X2 = 1.0f;
            static constexpr inline float       LINEAR_Y2 = 1.0f;

            static constexpr inline float       EASE_X1 = 0.25f;
            static constexpr inline float       EASE_Y1 = 0.1f;
            static constexpr inline float       EASE_X2 = 0.25f;
            static constexpr inline float       EASE_Y2 = 1.0f;

            static constexpr inline float       EASE_IN_X1 = 0.42f;
            static constexpr inline float       EASE_IN_Y1 = 0.0f;
            static constexpr inline float       EASE_IN_X2 = 1.0f;
            static constexpr inline float       EASE_IN_Y2 = 1.0f;

            static constexpr inline float       EASE_OUT_X1 = 0.0f;
            static constexpr inline float       EASE_OUT_Y1 = 0.0f;
            static constexpr inline float       EASE_OUT_X2 = 0.58f;
            static constexpr inline float       EASE_OUT_Y2 = 1.0f;

            static constexpr inline float       EASE_IN_OUT_X1 = 0.42f;
            static constexpr inline float       EASE_IN_OUT_Y1 = 0.0f;
            static constexpr inline float       EASE_IN_OUT_X2 = 0.58f;
            static constexpr inline float       EASE_IN_OUT_Y2 = 1.0f;

            static constexpr inline float       SOLVE_EPSILON              = 1.0e-6f;
            static constexpr inline int         SOLVE_NEWTON_ITERATIONS    = 8;
            static constexpr inline int         SOLVE_BISECTION_ITERATIONS = 16;

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
