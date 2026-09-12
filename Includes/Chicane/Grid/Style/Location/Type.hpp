#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleLocationType : std::uint8_t
        {
            Local,
            URL
        };

        inline String toString(StyleLocationType inValue)
        {
            switch (inValue)
            {
            case StyleLocationType::Local:
                return "Local";

            case StyleLocationType::URL:
                return "URL";

            default:
                return "";
            }
        }
    }
}