#include "Chicane/Grid/Style/Flex.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleFlex::StyleFlex()
            : direction(StyleFlexDirection::Row),
              wrap(StyleFlexWrap::NoWrap)
        {}

        void StyleFlex::refresh()
        {
            direction.refresh();
            wrap.refresh();
        }

        void StyleFlex::setProperties(const StyleRuleset::Properties& inProperties)
        {
            if (inProperties.find(Style::FLEX_DIRECTION_ATTRIBUTE_NAME) != inProperties.end())
            {
                direction.setRaw(inProperties.at(Style::FLEX_DIRECTION_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::FLEX_WRAP_ATTRIBUTE_NAME) != inProperties.end())
            {
                wrap.setRaw(inProperties.at(Style::FLEX_WRAP_ATTRIBUTE_NAME));
            }
        }

        void StyleFlex::parseWith(DirectionParser inDirectionParser, WrapParser inWrapParser)
        {
            direction.parseWith(inDirectionParser);
            wrap.parseWith(inWrapParser);
        }
    }
}
