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

    Chicane::String toString(NavigationType inValue);
}