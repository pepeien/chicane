#include "Chicane/Grid/Style/Alignment.hpp"

#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Alignment toAlignment(const std::string& inValue)
            {
                if (String::areEquals(inValue, ALIGNMENT_TYPE_CENTER))
                {
                    return Alignment::Center;
                }

                if (String::areEquals(inValue, ALIGNMENT_TYPE_START))
                {
                    return Alignment::Start;
                }

                return Alignment::End;
            }
        }
    }
}