#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Vendor
        {
            enum class Type : std::uint8_t
            {
                // N/A
                Undefined,
                // .obj
                Wavefront
            };
        }
    }
}