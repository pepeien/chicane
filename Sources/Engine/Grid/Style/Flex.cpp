#include "Chicane/Grid/Style/Flex.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleFlex::StyleFlex()
            : direction(StyleFlexDirection::Row)
        {}

        void StyleFlex::refresh()
        {
            direction.refresh();
        }

        void StyleFlex::setProperties(const StyleRuleset::Properties& inProperties)
        {
            if (inProperties.find(Style::FLEX_DIRECTION_ATTRIBUTE_NAME) != inProperties.end())
            {
                direction.setRaw(inProperties.at(Style::FLEX_DIRECTION_ATTRIBUTE_NAME));
            }
        }

        void StyleFlex::parseWith(DirectionParser inDirectionParser)
        {
            direction.parseWith(inDirectionParser);
        }
    }
}