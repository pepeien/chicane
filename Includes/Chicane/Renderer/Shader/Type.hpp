#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(Renderer::ShaderType inValue);
}