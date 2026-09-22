#include "Chicane/Core/Window/Cursor.hpp"

namespace Chicane
{
    String toString(WindowCursor inValue)
    {
        switch (inValue)
        {
        case WindowCursor::Default:
            return "Default";

        case WindowCursor::Pointer:
            return "Pointer";

        case WindowCursor::Text:
            return "Text";

        case WindowCursor::Crosshair:
            return "Crosshair";

        case WindowCursor::Move:
            return "Move";

        case WindowCursor::NotAllowed:
            return "NotAllowed";

        case WindowCursor::Wait:
            return "Wait";

        case WindowCursor::Progress:
            return "Progress";

        case WindowCursor::Grab:
            return "Grab";

        case WindowCursor::Grabbing:
            return "Grabbing";

        case WindowCursor::NsResize:
            return "NsResize";

        case WindowCursor::EwResize:
            return "EwResize";

        case WindowCursor::NeswResize:
            return "NeswResize";

        case WindowCursor::NwseResize:
            return "NwseResize";

        case WindowCursor::None:
            return "None";

        case WindowCursor::Count:
            return "Count";

        default:
            return "";
        }
    }
}
