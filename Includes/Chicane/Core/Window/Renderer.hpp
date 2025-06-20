#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    enum class WindowRenderer : std::uint8_t
    {
        Undefined,
        OpenGL,
        Vulkan
    };
}