#include "Chicane/Grid/Style/Font.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            void Font::update(const Properties &inSource)
            {
                if (inSource.find(FONT_FAMILY_ATTRIBUTE_NAME) != inSource.end())
                {
                    family = String::trim(inSource.at(FONT_FAMILY_ATTRIBUTE_NAME));
                }

                if (inSource.find(FONT_SIZE_ATTRIBUTE_NAME) != inSource.end())
                {
                    size = String::trim(inSource.at(FONT_SIZE_ATTRIBUTE_NAME));
                }
            }
        }
    }
}