#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class ListPushStrategy : std::uint8_t
    {
        Front,
        Back,
        Before,
        After,
        Replace
    };

    CHICANE_CORE String toString(ListPushStrategy inValue);
}