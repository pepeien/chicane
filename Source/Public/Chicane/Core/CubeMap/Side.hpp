#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        enum class Side : std::uint8_t
        {
            Left,
            Right,
            Front,
            Back,
            Up,
            Down
        };
    }
}