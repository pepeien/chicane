#include "Chicane/Grid/Style/WordBreak.hpp"

namespace Chicane
{
    String toString(Grid::StyleWordBreak inValue)
    {
        switch (inValue)
        {
        case Grid::StyleWordBreak::Normal:
            return "Normal";

        case Grid::StyleWordBreak::BreakWord:
            return "BreakWord";

        case Grid::StyleWordBreak::BreakAll:
            return "BreakAll";

        default:
            return "";
        }
    }
}
