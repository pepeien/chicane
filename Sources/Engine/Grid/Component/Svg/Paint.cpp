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
              strokeWidth(STROKE_WIDTH_DEFAULT_VALUE),
              opacity(OPACITY_DEFAULT_VALUE),
              fillOpacity(OPACITY_DEFAULT_VALUE),
              strokeOpacity(OPACITY_DEFAULT_VALUE),
              lineCap(SvgLineCap::Butt),
              lineJoin(SvgLineJoin::Miter),
              transform(1.0f)
        {}

        SvgLineCap SvgPaint::parseLineCap(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.equals(LINECAP_TYPE_ROUND))
            {
                return SvgLineCap::Round;
            }

            if (value.equals(LINECAP_TYPE_SQUARE))
            {
                return SvgLineCap::Square;
            }

            return SvgLineCap::Butt;
        }

        SvgLineJoin SvgPaint::parseLineJoin(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.equals(LINEJOIN_TYPE_ROUND))
            {
                return SvgLineJoin::Round;
            }

            if (value.equals(LINEJOIN_TYPE_BEVEL))
            {
                return SvgLineJoin::Bevel;
            }

            return SvgLineJoin::Miter;
        }
    }
}
