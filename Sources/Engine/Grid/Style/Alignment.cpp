#include "Chicane/Grid/Style/Alignment.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleAlignment toAlignment(const String& inValue)
        {
            const String value = inValue.trim().toLower();

            if (value.equals(Style::ALIGNMENT_TYPE_CENTER, "middle"))
            {
                return StyleAlignment::Center;
            }

            if (value.equals(Style::ALIGNMENT_TYPE_END, Style::ALIGNMENT_TYPE_RIGHT))
            {
                return StyleAlignment::End;
            }

            return StyleAlignment::Start;
        }
    }
}