#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Font/Glyph/Curve.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX FontGlyphCurveCollector
        {
        public:
            void moveTo(const Vec2& inPoint);
            void lineTo(const Vec2& inPoint);

            void quadraticTo(const Vec2& inControl, const Vec2& inPoint);
            void cubicTo(const Vec2& inControlA, const Vec2& inControlB, const Vec2& inPoint);

            void close();

        private:
            Vec2 getMidPoint(const Vec2& inLeft, const Vec2& inRight);

            void addCurve(const Vec2& inStart, const Vec2& inControl, const Vec2& inEnd);

            void subdivideCubic(
                const Vec2& inA, const Vec2& inB, const Vec2& inC, const Vec2& inD, std::uint32_t inDepth
            );

        public:
            bool                 bIsOpen = false;
            FontGlyphCurve::List curves  = {};
            Vec2                 cursor  = Vec2::Zero();
            Vec2                 contour = Vec2::Zero();
        };
    }
}