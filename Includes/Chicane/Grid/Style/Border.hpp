#pragma once

#include <algorithm>
#include <functional>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Size/Direction.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Border/Type.hpp"
#include "Chicane/Grid/Style/Corners.hpp"
#include "Chicane/Grid/Style/Property.hpp"
#include "Chicane/Grid/Style/Ruleset.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleBorder
        {
        public:
            using WidthParser = std::function<float(const String&, SizeDirection)>;
            using ColorParser = StyleProperty<Color::Rgba>::Parser;
            using TypeParser  = StyleProperty<StyleBorderType>::Parser;

        public:
            StyleBorder();

        public:
            void refresh();

            void setProperties(const StyleRuleset::Properties& inProperties);

            void parseWith(WidthParser inWidthParser, ColorParser inColorParser, TypeParser inTypeParser);

            float paintedTop() const;
            float paintedRight() const;
            float paintedBottom() const;
            float paintedLeft() const;
            Vec4 paintedWidths() const;

            bool isVisible() const;

        private:
            void applyShorthand(
                const String&                   inValue,
                StyleProperty<float>&           outWidth,
                StyleProperty<StyleBorderType>& outType,
                StyleProperty<Color::Rgba>&     outColor
            );
            void applyTypeOneliner(const String& inValue);
            void applyColorOneliner(const String& inValue);

            static bool isTypeToken(const String& inValue);
            static bool isColorToken(const String& inValue);

        public:
            StyleCorners                   width;
            StyleProperty<StyleBorderType> typeTop;
            StyleProperty<StyleBorderType> typeBottom;
            StyleProperty<StyleBorderType> typeLeft;
            StyleProperty<StyleBorderType> typeRight;
            StyleProperty<Color::Rgba>     colorTop;
            StyleProperty<Color::Rgba>     colorBottom;
            StyleProperty<Color::Rgba>     colorLeft;
            StyleProperty<Color::Rgba>     colorRight;
        };
    }
}
