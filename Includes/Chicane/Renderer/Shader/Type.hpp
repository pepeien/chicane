#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class ShaderType : std::uint8_t
        {
            Fragment,
            Vertex,
            Undefined
        };
    }
}