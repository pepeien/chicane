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
    }
}