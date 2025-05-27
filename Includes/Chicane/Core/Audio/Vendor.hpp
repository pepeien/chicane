#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Audio
    {
        enum class Vendor : std::uint8_t
        {
            Undefined, // N/A
            Wave       // .wav
        };
    }
}