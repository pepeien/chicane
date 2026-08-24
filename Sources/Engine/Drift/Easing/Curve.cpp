#include "Chicane/Drift/Easing/Curve.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace Chicane
{
    namespace Drift
    {
        EasingCurve EasingCurve::linear()
        {
            return EasingCurve(Easing::Linear);
        }

        EasingCurve EasingCurve::ease()
        {
            return EasingCurve(EASE_X1, EASE_Y1, EASE_X2, EASE_Y2);
        }

        EasingCurve EasingCurve::easeIn()
        {
            return EasingCurve(EASE_IN_X1, EASE_IN_Y1, EASE_IN_X2, EASE_IN_Y2);
        }

        EasingCurve EasingCurve::easeOut()
        {
            return EasingCurve(EASE_OUT_X1, EASE_OUT_Y1, EASE_OUT_X2, EASE_OUT_Y2);
        }

        EasingCurve EasingCurve::easeInOut()
        {
            return EasingCurve(EASE_IN_OUT_X1, EASE_IN_OUT_Y1, EASE_IN_OUT_X2, EASE_IN_OUT_Y2);
        }

        EasingCurve EasingCurve::cubicBezier(float inX1, float inY1, float inX2, float inY2)
        {
            return EasingCurve(inX1, inY1, inX2, inY2);
        }

        EasingCurve EasingCurve::fromString(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.equals(TYPE_LINEAR))
            {
                return linear();
            }

            if (value.equals(TYPE_EASE))
            {
                return ease();
            }

            if (value.equals(TYPE_EASE_IN))
            {
                return easeIn();
            }

            if (value.equals(TYPE_EASE_OUT))
            {
                return easeOut();
            }

            if (value.equals(TYPE_EASE_IN_OUT))
            {
                return easeInOut();
            }

            if (value.startsWith(CUBIC_BEZIER_KEYWORD))
            {
                const String              raw   = value.getBetween('(', ')');
                const std::vector<String> parts = raw.split(',');

                if (parts.size() >= 4)
                {
                    auto number = [](const String& inToken) -> float
                    {
                        const String token = inToken.trim();

                        if (token.isEmpty() || token.isNaN())
                        {
                            return 0.0f;
                        }

                        return static_cast<float>(std::strtod(token.toChar(), nullptr));
                    };

                    return cubicBezier(
                        number(parts.at(0)),
                        number(parts.at(1)),
                        number(parts.at(2)),
                        number(parts.at(3))
                    );
                }
            }

            return ease();
        }

        EasingCurve::EasingCurve()
            : EasingCurve(Easing::Ease)
        {}

        EasingCurve::EasingCurve(Easing inType)
            : type(inType),
              x1(LINEAR_X1),
              y1(LINEAR_Y1),
              x2(LINEAR_X2),
              y2(LINEAR_Y2)
        {
            switch (inType)
            {
            case Easing::Linear:
                x1 = LINEAR_X1;
                y1 = LINEAR_Y1;
                x2 = LINEAR_X2;
                y2 = LINEAR_Y2;
                break;

            case Easing::Ease:
                *this = ease();
                break;

            case Easing::EaseIn:
                *this = easeIn();
                break;

            case Easing::EaseOut:
                *this = easeOut();
                break;

            case Easing::EaseInOut:
                *this = easeInOut();
                break;

            default:
                break;
            }
        }

        EasingCurve::EasingCurve(float inX1, float inY1, float inX2, float inY2)
            : type(Easing::CubicBezier),
              x1(inX1),
              y1(inY1),
              x2(inX2),
              y2(inY2)
        {}

        float EasingCurve::evaluate(float inTime) const
        {
            const float time = std::clamp(inTime, 0.0f, 1.0f);

            if (type == Easing::Linear || time <= 0.0f || time >= 1.0f)
            {
                return time;
            }

            return sampleY(solve(time));
        }

        float EasingCurve::sampleX(float inT) const
        {
            const float oneMinusT = 1.0f - inT;

            return (3.0f * oneMinusT * oneMinusT * inT * x1) + (3.0f * oneMinusT * inT * inT * x2) + (inT * inT * inT);
        }

        float EasingCurve::sampleY(float inT) const
        {
            const float oneMinusT = 1.0f - inT;

            return (3.0f * oneMinusT * oneMinusT * inT * y1) + (3.0f * oneMinusT * inT * inT * y2) + (inT * inT * inT);
        }

        float EasingCurve::sampleDerivativeX(float inT) const
        {
            const float oneMinusT = 1.0f - inT;

            return (3.0f * oneMinusT * oneMinusT * x1) + (6.0f * oneMinusT * inT * (x2 - x1)) +
                   (3.0f * inT * inT * (1.0f - x2));
        }

        float EasingCurve::solve(float inTime) const
        {
            float guess = inTime;

            for (int i = 0; i < SOLVE_NEWTON_ITERATIONS; i++)
            {
                const float current    = sampleX(guess) - inTime;
                const float derivative = sampleDerivativeX(guess);

                if (std::fabs(current) < SOLVE_EPSILON)
                {
                    return guess;
                }

                if (std::fabs(derivative) < SOLVE_EPSILON)
                {
                    break;
                }

                guess = std::clamp(guess - (current / derivative), 0.0f, 1.0f);
            }

            float min = 0.0f;
            float max = 1.0f;
            guess     = inTime;

            for (int i = 0; i < SOLVE_BISECTION_ITERATIONS; i++)
            {
                const float current = sampleX(guess);

                if (std::fabs(current - inTime) < SOLVE_EPSILON)
                {
                    return guess;
                }

                if (inTime > current)
                {
                    min = guess;
                }
                else
                {
                    max = guess;
                }

                guess = (min + max) * 0.5f;
            }

            return guess;
        }
    }
}
