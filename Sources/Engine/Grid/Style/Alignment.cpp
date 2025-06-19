#include "Chicane/Grid/Style/Alignment.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleAlignment toAlignment(const String& inValue)
        {
            if (inValue.equals(Style::ALIGNMENT_TYPE_CENTER))
            {
                return StyleAlignment::Center;
            }

            if (inValue.equals(Style::ALIGNMENT_TYPE_START))
            {
                return StyleAlignment::Start;
            }

            return StyleAlignment::End;
        }
    }
}