#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleLocationType : std::uint8_t
        {
            Local,
            URL
        };
    }
}