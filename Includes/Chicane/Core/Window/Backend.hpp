#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class WindowBackend : std::uint8_t
    {
        Undefined,
#if CHICANE_OPENGL
        OpenGL,
#endif
#if CHICANE_VULKAN
        Vulkan,
#endif
    };

    CHICANE_CORE String toString(WindowBackend inValue);
}