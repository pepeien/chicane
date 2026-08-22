#include "Chicane/Box/Font/Glyph/Curve/Collector.hpp"

namespace Chicane
{
    namespace Box
    {
        void FontGlyphCurveCollector::moveTo(const Vec2& inPoint)
        {
            close();

            cursor  = inPoint;
            contour = inPoint;
            bIsOpen = true;
        }

        void FontGlyphCurveCollector::lineTo(const Vec2& inPoint)
        {
            addCurve(cursor, getMidPoint(cursor, inPoint), inPoint);
        }

        void FontGlyphCurveCollector::quadraticTo(const Vec2& inControl, const Vec2& inPoint)
        {
            addCurve(cursor, inControl, inPoint);
        }

        void FontGlyphCurveCollector::cubicTo(const Vec2& inControlA, const Vec2& inControlB, const Vec2& inPoint)
        {
            subdivideCubic(cursor, inControlA, inControlB, inPoint, 3);
        }

        void FontGlyphCurveCollector::close()
        {
            if (!bIsOpen)
            {
                return;
            }

            if (cursor.x != contour.x || cursor.y != contour.y)
            {
                lineTo(contour);
            }

            bIsOpen = false;
        }

        Vec2 FontGlyphCurveCollector::getMidPoint(const Vec2& inLeft, const Vec2& inRight)
        {
            return Vec2((inLeft.x + inRight.x) * 0.5f, (inLeft.y + inRight.y) * 0.5f);
        }

        void FontGlyphCurveCollector::addCurve(const Vec2& inStart, const Vec2& inControl, const Vec2& inEnd)
        {
            FontGlyphCurve curve;
            curve.start   = inStart;
            curve.control = inControl;
            curve.end     = inEnd;

            curves.push_back(curve);

            cursor = inEnd;
        }

        void FontGlyphCurveCollector::subdivideCubic(
            const Vec2& inA, const Vec2& inB, const Vec2& inC, const Vec2& inD, std::uint32_t inDepth
        )
        {
            if (inDepth == 0)
            {
                const Vec2 control(
                    ((3.0f * (inB.x + inC.x)) - (inA.x + inD.x)) * 0.25f,
                    ((3.0f * (inB.y + inC.y)) - (inA.y + inD.y)) * 0.25f
                );

                addCurve(inA, control, inD);

                return;
            }

            const Vec2 ab   = getMidPoint(inA, inB);
            const Vec2 bc   = getMidPoint(inB, inC);
            const Vec2 cd   = getMidPoint(inC, inD);
            const Vec2 abc  = getMidPoint(ab, bc);
            const Vec2 bcd  = getMidPoint(bc, cd);
            const Vec2 abcd = getMidPoint(abc, bcd);

            subdivideCubic(inA, ab, abc, abcd, inDepth - 1);
            subdivideCubic(abcd, bcd, cd, inD, inDepth - 1);
        }
    }
}