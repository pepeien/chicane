#include "Chicane/Grid/Style/Radius/Corners.hpp"

#include <algorithm>

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace
        {
            void assignCorners(
                const std::vector<String>& inValues,
                String&                    outTopLeft,
                String&                    outTopRight,
                String&                    outBottomRight,
                String&                    outBottomLeft
            )
            {
                if (inValues.empty())
                {
                    return;
                }

                if (inValues.size() == 1)
                {
                    outTopLeft     = inValues.at(0);
                    outTopRight    = inValues.at(0);
                    outBottomRight = inValues.at(0);
                    outBottomLeft  = inValues.at(0);

                    return;
                }

                if (inValues.size() == 2)
                {
                    outTopLeft     = inValues.at(0);
                    outBottomRight = inValues.at(0);
                    outTopRight    = inValues.at(1);
                    outBottomLeft  = inValues.at(1);

                    return;
                }

                if (inValues.size() == 3)
                {
                    outTopLeft     = inValues.at(0);
                    outTopRight    = inValues.at(1);
                    outBottomLeft  = inValues.at(1);
                    outBottomRight = inValues.at(2);

                    return;
                }

                outTopLeft     = inValues.at(0);
                outTopRight    = inValues.at(1);
                outBottomRight = inValues.at(2);
                outBottomLeft  = inValues.at(3);
            }
        }

        void StyleRadiusCorners::parseWith(
            std::function<float(const String&, SizeDirection)> inParser, SizeDirection inDirection
        )
        {
            const auto parser = [inParser, inDirection](const String& inValue)
            { return std::max(0.0f, inParser(inValue, inDirection)); };

            top.parseWith(parser);
            right.parseWith(parser);
            bottom.parseWith(parser);
            left.parseWith(parser);
        }

        void StyleRadiusCorners::setOnelinerAttribute(const String& inValue)
        {
            String topLeft     = Style::CORNER_DEFAULT_VALUE;
            String topRight    = Style::CORNER_DEFAULT_VALUE;
            String bottomRight = Style::CORNER_DEFAULT_VALUE;
            String bottomLeft  = Style::CORNER_DEFAULT_VALUE;

            assignCorners(splitOneliner(inValue), topLeft, topRight, bottomRight, bottomLeft);

            top.setRaw(topLeft.trim());
            right.setRaw(topRight.trim());
            bottom.setRaw(bottomRight.trim());
            left.setRaw(bottomLeft.trim());
        }

        Vec4 StyleRadiusCorners::values() const
        {
            return Vec4(top.get(), right.get(), bottom.get(), left.get());
        }

        bool StyleRadiusCorners::isZero() const
        {
            return top.get() <= 0.0f && right.get() <= 0.0f && bottom.get() <= 0.0f && left.get() <= 0.0f;
        }
    }
}
