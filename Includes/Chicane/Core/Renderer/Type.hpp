#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class Type : std::uint8_t
        {
            Undefined,
            OpenGL,
            Vulkan
        };
    }
}