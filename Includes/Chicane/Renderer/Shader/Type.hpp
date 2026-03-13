#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"

namespace Chicane
{
    namespace Renderer
    {
        CH_ENUM()
        enum class ShaderType : std::uint8_t
        {
            Fragment,
            Vertex,
            Undefined
        };
    }
}