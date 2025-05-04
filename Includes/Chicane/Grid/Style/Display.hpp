#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            enum class Display : std::uint8_t
            {
                Visible, // Visible and occupies space on the overall layout
                Hidden,  // Visibly hidden but still occupies space on the overall layout
                None     // Visibly hidden and doens't occupiy space on the overall layout
            };
        }
    }
}