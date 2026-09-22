#include "Chicane/Core/List/Push/Strategy.hpp"

namespace Chicane
{
    String toString(ListPushStrategy inValue)
    {
        switch (inValue)
        {
        case ListPushStrategy::Front:
            return "Front";

        case ListPushStrategy::Back:
            return "Back";

        case ListPushStrategy::Before:
            return "Before";

        case ListPushStrategy::After:
            return "After";

        case ListPushStrategy::Replace:
            return "Replace";

        default:
            return "";
        }
    }
}
