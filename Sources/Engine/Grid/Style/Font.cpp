#include "Chicane/Grid/Style/Font.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleFont::StyleFont()
            : family(Style::FONT_FAMILY_DEFAULT_VALUE),
              size(16.0f)
        {}

        void StyleFont::refresh()
        {
            family.refresh();
            size.refresh();
        }

        void StyleFont::setProperties(const StyleRuleset::Properties& inProperties)
        {
            if (inProperties.find(Style::FONT_FAMILY_ATTRIBUTE_NAME) != inProperties.end())
            {
                family.setRaw(inProperties.at(Style::FONT_FAMILY_ATTRIBUTE_NAME));
            }

            if (inProperties.find(Style::FONT_SIZE_ATTRIBUTE_NAME) != inProperties.end())
            {
                size.setRaw(inProperties.at(Style::FONT_SIZE_ATTRIBUTE_NAME));
            }
        }

        void StyleFont::parseWith(FamilyParser inFamilyParser, SizeParser inSizeParser)
        {
            family.parseWith(inFamilyParser);
            size.parseWith(inSizeParser);
        }
    }
}