#pragma once

#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Size/Direction.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Radius/Corners.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleRadius
        {
        public:
            StyleRadius();

        public:
            friend inline bool operator==(const StyleRadius& inLeft, const StyleRadius& inRight)
            {
                return inLeft.x == inRight.x && inLeft.y == inRight.y;
            }

        public:
            void refresh();
            void constrain(float inWidth, float inHeight);

            void setProperties(
                const StyleRuleset::Properties& inProperties,
                const String&                   inOnelineAttributeName,
                const String&                   inTopLeftAttributeName,
                const String&                   inTopRightAttributeName,
                const String&                   inBottomRightAttributeName,
                const String&                   inBottomLeftAttributeName
            );

            void parseWith(std::function<float(const String&, SizeDirection)> inParser);

            Vec4 horizontal() const;
            Vec4 vertical() const;

            bool isZero() const;

        private:
            void setOnelinerAttribute(const String& inValue);
            void setCornerRaw(StyleProperty<float>& outHorizontal, StyleProperty<float>& outVertical, const String& inValue);

        public:
            StyleRadiusCorners x;
            StyleRadiusCorners y;
        };
    }
}
