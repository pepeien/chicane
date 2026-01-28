#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class ViewProjectionType : std::uint8_t
    {
        Orthographic,
        Perspective
    };
}