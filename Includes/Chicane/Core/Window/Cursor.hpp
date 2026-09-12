#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class WindowCursor : std::uint8_t
    {
        Default,
        Pointer,
        Text,
        Crosshair,
        Move,
        NotAllowed,
        Wait,
        Progress,
        Grab,
        Grabbing,
        NsResize,
        EwResize,
        NeswResize,
        NwseResize,
        None,
        Count
    };

    inline String toString(WindowCursor inValue)
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
