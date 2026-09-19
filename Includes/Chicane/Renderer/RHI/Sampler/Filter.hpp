#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class SamplerFilter : std::uint8_t
            {
                Nearest,
                Linear
            };
        }
    }

    inline String toString(Renderer::RHI::SamplerFilter inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::SamplerFilter::Nearest:
            return "Nearest";

        case Renderer::RHI::SamplerFilter::Linear:
            return "Linear";

        default:
            return "";
        }
    }
}
