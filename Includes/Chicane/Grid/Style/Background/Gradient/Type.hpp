#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleGradientType : std::int32_t
        {
            None   = 0,
            Linear = 1,
            Radial = 2
        };
    }

    inline String toString(Grid::StyleGradientType inValue)
    {
        switch (inValue)
        {
        case Grid::StyleGradientType::Linear:
            return "Linear";

        case Grid::StyleGradientType::Radial:
            return "Radial";

        default:
            return "None";
        }
    }
}
