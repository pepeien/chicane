#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class BlendMode : std::uint8_t
            {
                None,
                Alpha,
                Additive
            };
        }
    }

    inline String toString(Renderer::RHI::BlendMode inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::BlendMode::None:
            return "None";

        case Renderer::RHI::BlendMode::Alpha:
            return "Alpha";

        case Renderer::RHI::BlendMode::Additive:
            return "Additive";

        default:
            return "";
        }
    }
}
