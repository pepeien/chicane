#include "Chicane/Grid/Style/Location.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleLocation::StyleLocation()
            : type(StyleLocationType::Local),
              value("")
        {}

        void StyleLocation::parse(const String& inValue)
        {
            if (!inValue.startsWith(Style::URL_KEYWORD))
            {
                type  = StyleLocationType::Local;
                value = inValue;

                return;
            }

            type  = StyleLocationType::URL;
            value = inValue.getBetween(FUNCTION_PARAMS_OPENING, FUNCTION_PARAMS_CLOSING);
        }
    }
}