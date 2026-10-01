#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    enum class ObjectOrigin : std::uint8_t
    {
        Native,
        Instance,
        Spawned,
        Transient
    };

    CHICANE_RUNTIME String toString(ObjectOrigin inValue);
}
