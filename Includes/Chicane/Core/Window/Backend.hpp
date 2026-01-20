#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class WindowBackend : std::uint8_t
    {
        Undefined,
        OpenGL,
        Vulkan
    };
}