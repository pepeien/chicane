#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID SvgPaint
        {
        public:
            SvgPaint();
            SvgPaint(const Color::Rgba& inColor);

        public:
            bool        bIsStrokeEnabled;
            bool        bIsEvenOdd;
            bool        bIsFillEnabled;

            Color::Rgba fill;
            Color::Rgba stroke;
            float       strokeWidth;
            float       opacity;
            float       fillOpacity;
            float       strokeOpacity;
            Mat3        transform;
        };
    }
}
