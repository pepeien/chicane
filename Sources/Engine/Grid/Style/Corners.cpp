#include "Chicane/Grid/Style/Corners.hpp"

#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleCorners::StyleCorners()
            : top(0.0f),
              bottom(0.0f),
              left(0.0f),
              right(0.0f)
        {}

        void StyleCorners::refresh()
        {
            top.refresh();
            left.refresh();
            bottom.refresh();
            right.refresh();
        }

        void StyleCorners::setProperties(
            const StyleRuleset::Properties& inProperties,
            const String&                   inOnelineAttributeName,
            const String&                   inTopAttributeName,
            const String&                   inBottomAttributeName,
            const String&                   inLeftAttributeName,
            const String&                   inRightAttributeName
        )
        {
            if (inProperties.find(inOnelineAttributeName) != inProperties.end())
            {
                setOnelinerAttribute(inProperties.at(inOnelineAttributeName));
            }

            if (inProperties.find(inTopAttributeName) != inProperties.end())
            {
                top.setRaw(inProperties.at(inTopAttributeName));
            }

            if (inProperties.find(inBottomAttributeName) != inProperties.end())
            {
                bottom.setRaw(inProperties.at(inBottomAttributeName));
            }

            if (inProperties.find(inLeftAttributeName) != inProperties.end())
            {
                left.setRaw(inProperties.at(inLeftAttributeName));
            }

            if (inProperties.find(inRightAttributeName) != inProperties.end())
            {
                right.setRaw(inProperties.at(inRightAttributeName));
            }
        }

        void StyleCorners::parseWith(std::function<float(const String&, SizeDirection)> inParser)
        {
            top.parseWith([inParser](const String& inValue) { return inParser(inValue, SizeDirection::Vertical); });
            left.parseWith([inParser](const String& inValue) { return inParser(inValue, SizeDirection::Horizontal); });
            bottom.parseWith([inParser](const String& inValue) { return inParser(inValue, SizeDirection::Vertical); });
            right.parseWith([inParser](const String& inValue) { return inParser(inValue, SizeDirection::Horizontal); });
        }

        void StyleCorners::setAll(float inValue)
        {
            top.set(inValue);
            bottom.set(inValue);
            left.set(inValue);
            right.set(inValue);
        }

        void StyleCorners::setOnelinerAttribute(const String& inValue)
        {
            String topValue    = Style::CORNER_DEFAULT_VALUE;
            String rightValue  = Style::CORNER_DEFAULT_VALUE;
            String bottomValue = Style::CORNER_DEFAULT_VALUE;
            String leftValue   = Style::CORNER_DEFAULT_VALUE;

            const std::vector<String> values = splitOneliner(inValue);

            if (values.size() == 1) // SINGLE
            {
                const String& value = values.at(0);

                topValue    = value;
                rightValue  = value;
                bottomValue = value;
                leftValue   = value;
            }

            if (values.size() == 2) // VERTICAL HORIZONTAL
            {
                const String& vertical   = values.at(0);
                const String& horizontal = values.at(1);

                topValue    = vertical;
                bottomValue = vertical;
                rightValue  = horizontal;
                leftValue   = horizontal;
            }

            if (values.size() == 3) // TOP BOTTOM HORIZONTAL
            {
                const String& horizontal = values.at(2);

                topValue    = values.at(0);
                bottomValue = values.at(1);
                rightValue  = horizontal;
                leftValue   = horizontal;
            }

            if (values.size() >= 4) // TOP RIGHT BOTTOM LEFT
            {
                topValue    = values.at(0);
                rightValue  = values.at(1);
                bottomValue = values.at(2);
                leftValue   = values.at(3);
            }

            top.setRaw(topValue.trim());
            bottom.setRaw(bottomValue.trim());
            left.setRaw(leftValue.trim());
            right.setRaw(rightValue.trim());
        }
    }
}