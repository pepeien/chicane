#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleBorderType : std::uint8_t
        {
            None,
            Solid
        };

        inline String toString(StyleBorderType inValue)
        {
            switch (inValue)
            {
            case StyleBorderType::None:
                return "None";

            case StyleBorderType::Solid:
                return "Solid";

            default:
                return "";
            }
        }
    }
}
