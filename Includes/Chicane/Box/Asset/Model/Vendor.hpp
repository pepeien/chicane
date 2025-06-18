#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class ModelVendor : std::uint8_t
        {
            Undefined, // N/A
            Wavefront  // .obj
        };
    }
}