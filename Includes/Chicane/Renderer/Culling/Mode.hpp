#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class CullingMode : std::uint8_t
        {
            None,
            Front,
            Back,
            FrontAndBack
        };
    }

    inline String toString(Renderer::CullingMode inValue)
    {
        switch (inValue)
        {
        case Renderer::CullingMode::None:
            return "None";

        case Renderer::CullingMode::Front:
            return "Front";

        case Renderer::CullingMode::Back:
            return "Back";

        case Renderer::CullingMode::FrontAndBack:
            return "FrontAndBack";

        default:
            return "";
        }
    }
}
