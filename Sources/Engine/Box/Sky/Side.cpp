#include "Chicane/Box/Sky/Side.hpp"

namespace Chicane
{
    String toString(Box::SkySide inValue)
    {
        switch (inValue)
        {
        case Box::SkySide::Right:
            return "Right";

        case Box::SkySide::Left:
            return "Left";

        case Box::SkySide::Front:
            return "Front";

        case Box::SkySide::Back:
            return "Back";

        case Box::SkySide::Up:
            return "Up";

        case Box::SkySide::Down:
            return "Down";

        default:
            return "";
        }
    }
}
