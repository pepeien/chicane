#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            enum class Vendor : std::uint8_t
            {
                Undefined, // N/A
                Wavefront  // .obj
            };
        }
    }
}