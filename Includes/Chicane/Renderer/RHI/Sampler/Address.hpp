#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class SamplerAddress : std::uint8_t
            {
                ClampToEdge,
                ClampToBorder,
                Repeat
            };
        }
    }

    inline String toString(Renderer::RHI::SamplerAddress inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::SamplerAddress::ClampToEdge:
            return "ClampToEdge";

        case Renderer::RHI::SamplerAddress::ClampToBorder:
            return "ClampToBorder";

        case Renderer::RHI::SamplerAddress::Repeat:
            return "Repeat";

        default:
            return "";
        }
    }
}
