#pragma once

#include <cstdint>

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class SkyKind : std::uint8_t
        {
            Cube,
            Panorama
        };
    }

    CHICANE_BOX String toString(Box::SkyKind inValue);
}
