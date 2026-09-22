#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_CORE String toString(Input::Status inValue);
}
