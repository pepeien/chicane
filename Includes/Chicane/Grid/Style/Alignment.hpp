#pragma once

#include "Chicane/Grid/Style/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            enum class Alignment : std::uint8_t
            {
                Start,
                Center,
                End
            };

            Alignment toAlignment(const std::string& inText);
        }
    }
}