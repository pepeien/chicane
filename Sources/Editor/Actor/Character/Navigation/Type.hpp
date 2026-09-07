#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>

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
}
