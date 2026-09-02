#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Svg/Paint/LineCap.hpp"
#include "Chicane/Grid/Component/Svg/Paint/LineJoin.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID SvgPaint
        {
        public:
            // Fill
            static constexpr inline const char* FILL_ATTRIBUTE_NAME         = "fill";
            static constexpr inline const char* FILL_OPACITY_ATTRIBUTE_NAME = "fill-opacity";
            static constexpr inline const char* FILL_RULE_ATTRIBUTE_NAME    = "fill-rule";
            static constexpr inline const char* FILL_RULE_TYPE_EVENODD      = "evenodd";

            // Stroke
            static constexpr inline const char* STROKE_ATTRIBUTE_NAME          = "stroke";
            static constexpr inline const char* STROKE_WIDTH_ATTRIBUTE_NAME    = "stroke-width";
            static constexpr inline const char* STROKE_OPACITY_ATTRIBUTE_NAME  = "stroke-opacity";
            static constexpr inline const char* STROKE_LINECAP_ATTRIBUTE_NAME  = "stroke-linecap";
            static constexpr inline const char* STROKE_LINEJOIN_ATTRIBUTE_NAME = "stroke-linejoin";
            static constexpr inline const char* LINECAP_TYPE_BUTT              = "butt";
            static constexpr inline const char* LINECAP_TYPE_ROUND             = "round";
            static constexpr inline const char* LINECAP_TYPE_SQUARE            = "square";
            static constexpr inline const char* LINEJOIN_TYPE_MITER            = "miter";
            static constexpr inline const char* LINEJOIN_TYPE_ROUND            = "round";
            static constexpr inline const char* LINEJOIN_TYPE_BEVEL            = "bevel";
            static constexpr inline float       STROKE_WIDTH_DEFAULT_VALUE     = 1.0f;
            static constexpr inline float       STROKE_MITER_LIMIT             = 4.0f;
            static constexpr inline float       STROKE_JOIN_MIN_LENGTH         = 1.0e-4f;
            static constexpr inline float       STROKE_MITER_COSINE_MIN        = 0.15f;
            static constexpr inline float       STROKE_MITER_COSINE_MAX        = 1.0f;
            static constexpr inline int         STROKE_ARC_SEGMENTS            = 16;

            // Opacity
            static constexpr inline const char* OPACITY_ATTRIBUTE_NAME = "opacity";
            static constexpr inline float       OPACITY_DEFAULT_VALUE  = 1.0f;

            // Transform
            static constexpr inline const char* TRANSFORM_ATTRIBUTE_NAME = "transform";
            static constexpr inline const char* STYLE_ATTRIBUTE_NAME     = "style";

        public:
            SvgPaint();
            SvgPaint(const Color::Rgba& inColor);

        public:
            static SvgLineCap parseLineCap(const String& inValue);
            static SvgLineJoin parseLineJoin(const String& inValue);

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
            SvgLineCap  lineCap;
            SvgLineJoin lineJoin;
            Mat3        transform;
        };
    }
}
