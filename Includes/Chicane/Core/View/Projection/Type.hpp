#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    CH_ENUM()
    enum class ViewProjectionType : std::uint8_t
    {
        Orthographic,
        Perspective
    };

    CHICANE_CORE String toString(ViewProjectionType inValue);
}