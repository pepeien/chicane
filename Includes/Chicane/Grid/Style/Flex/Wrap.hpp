#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleFlexWrap : std::uint8_t
        {
            NoWrap,
            Wrap
        };

        inline String toString(StyleFlexWrap inValue)
        {
            switch (inValue)
            {
            case StyleFlexWrap::NoWrap:
                return "NoWrap";

            case StyleFlexWrap::Wrap:
                return "Wrap";

            default:
                return "";
            }
        }
    }
}
