#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class ViewProjectionType : std::uint8_t
    {
        Orthographic,
        Perspective
    };

    CHICANE_CORE String toString(ViewProjectionType inValue);
}