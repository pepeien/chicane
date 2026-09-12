#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class SizeDirection : std::uint8_t
    {
        Vertical,
        Horizontal
    };

    inline String toString(SizeDirection inValue)
    {
        switch (inValue)
        {
        case SizeDirection::Vertical:
            return "Vertical";

        case SizeDirection::Horizontal:
            return "Horizontal";

        default:
            return "";
        }
    }
}