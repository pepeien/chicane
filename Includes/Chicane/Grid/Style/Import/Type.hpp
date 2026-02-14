#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleImportType : std::uint8_t
        {
            Undefined,
            Style,
            Font,
            Texture
        };
    }
}