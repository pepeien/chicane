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

    Chicane::String toString(CoordinateSpace inValue);

    inline CoordinateSpace parseCoordinateSpace(const Chicane::String& inValue)
    {
        if (inValue.equals("Relative"))
        {
            return CoordinateSpace::Relative;
        }

        return CoordinateSpace::Absolute;
    }
}
