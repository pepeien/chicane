#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_ENUM()
    enum class NavigationType : std::uint8_t
    {
        None,
        Orbit,
        Pan,
        Zoom
    };

    inline Chicane::String toString(NavigationType inValue)
    {
        switch (inValue)
        {
        case NavigationType::None:
            return "None";

        case NavigationType::Orbit:
            return "Orbit";

        case NavigationType::Pan:
            return "Pan";

        case NavigationType::Zoom:
            return "Zoom";

        default:
            return "";
        }
    }
}