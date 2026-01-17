#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Input
    {
        enum class Status : std::uint8_t
        {
            Pressed,
            Released
        };
    }
}