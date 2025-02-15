#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Vendor
            {
                enum class Type : std::uint8_t
                {
                    Undefined, // N/A
                    Wavefront  // .obj
                };
            }
        }
    }
}