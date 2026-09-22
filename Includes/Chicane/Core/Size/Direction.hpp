#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class SizeDirection : std::uint8_t
    {
        Vertical,
        Horizontal
    };

    CHICANE_CORE String toString(SizeDirection inValue);
}