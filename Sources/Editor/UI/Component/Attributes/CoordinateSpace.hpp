#pragma once

#include <cstdint>

#include <Chicane/Core/String.hpp>

namespace Editor
{
    enum class CoordinateSpace : std::uint8_t
    {
        Absolute,
        Relative
    };

    inline Chicane::String toString(CoordinateSpace inValue)
    {
        switch (inValue)
        {
        case CoordinateSpace::Relative:
            return "Relative";

        case CoordinateSpace::Absolute:
        default:
            return "Absolute";
        }
    }

    inline CoordinateSpace parseCoordinateSpace(const Chicane::String& inValue)
    {
        if (inValue.equals("Relative"))
        {
            return CoordinateSpace::Relative;
        }

        return CoordinateSpace::Absolute;
    }
}
