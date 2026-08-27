#include "Chicane/Grid/Component/Svg/Paint.hpp"

namespace Chicane
{
    namespace Grid
    {
        SvgPaint::SvgPaint()
            : SvgPaint(Color::toRgba(Color::TEXT_COLOR_BLACK))
        {}

        SvgPaint::SvgPaint(const Color::Rgba& inColor)
            : bIsStrokeEnabled(false),
              bIsEvenOdd(false),
              bIsFillEnabled(true),
              fill(inColor),
              stroke(inColor),
              strokeWidth(1.0f),
              opacity(1.0f),
              fillOpacity(1.0f),
              strokeOpacity(1.0f),
              transform(1.0f)
        {}
    }
}
