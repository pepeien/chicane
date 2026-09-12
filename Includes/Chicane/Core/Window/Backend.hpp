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

    inline String toString(WindowBackend inValue)
    {
        switch (inValue)
        {
        case WindowBackend::Undefined:
            return "Undefined";

        case WindowBackend::OpenGL:
            return "OpenGL";

        case WindowBackend::Vulkan:
            return "Vulkan";

        default:
            return "";
        }
    }
}