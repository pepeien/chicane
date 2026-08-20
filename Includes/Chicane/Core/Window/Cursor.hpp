#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

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
}
