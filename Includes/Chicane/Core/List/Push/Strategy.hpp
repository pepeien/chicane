#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class ListPushStrategy : std::uint8_t
    {
        Front,
        Back,
        Before,
        After
    };
}