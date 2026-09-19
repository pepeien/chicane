#pragma once

#include <cstdint>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

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

    inline String toString(Renderer::ShaderType inValue)
    {
        switch (inValue)
        {
        case Renderer::ShaderType::Fragment:
            return "Fragment";

        case Renderer::ShaderType::Vertex:
            return "Vertex";

        case Renderer::ShaderType::Undefined:
            return "Undefined";

        default:
            return "";
        }
    }
}