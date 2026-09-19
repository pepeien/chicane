#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class StencilMode : std::uint8_t
            {
                None,
                WriteReplace,
                TestNotEqual
            };
        }
    }

    inline String toString(Renderer::RHI::StencilMode inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::StencilMode::None:
            return "None";

        case Renderer::RHI::StencilMode::WriteReplace:
            return "WriteReplace";

        case Renderer::RHI::StencilMode::TestNotEqual:
            return "TestNotEqual";

        default:
            return "";
        }
    }
}
