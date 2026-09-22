#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        enum class Planet : std::uint8_t
        {
            Mercury,
            Venus,
            Earth,
            Moon,
            Mars,
            Jupiter,
            Saturn,
            Uranus,
            Neptune,
            Pluto
        };
    }

    CHICANE_KERB String toString(Kerb::Planet inValue);
}
