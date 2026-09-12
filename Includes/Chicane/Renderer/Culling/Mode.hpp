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

        inline String toString(CullingMode inValue)
        {
            switch (inValue)
            {
            case CullingMode::None:
                return "None";

            case CullingMode::Front:
                return "Front";

            case CullingMode::Back:
                return "Back";

            case CullingMode::FrontAndBack:
                return "FrontAndBack";

            default:
                return "";
            }
        }
    }
}