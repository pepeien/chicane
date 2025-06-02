#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Window
    {
        enum class Renderer : std::uint8_t
        {
            Undefined,
            OpenGL,
            Vulkan
        };
    }
}