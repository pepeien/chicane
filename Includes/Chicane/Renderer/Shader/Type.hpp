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

        inline String toString(ShaderType inValue)
        {
            switch (inValue)
            {
            case ShaderType::Fragment:
                return "Fragment";

            case ShaderType::Vertex:
                return "Vertex";

            case ShaderType::Undefined:
                return "Undefined";

            default:
                return "";
            }
        }
    }
}