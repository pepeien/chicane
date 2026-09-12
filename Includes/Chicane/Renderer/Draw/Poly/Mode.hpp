#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPolyMode : std::uint8_t
        {
            Fill,
            Line
        };

        inline String toString(DrawPolyMode inValue)
        {
            switch (inValue)
            {
            case DrawPolyMode::Fill:
                return "Fill";

            case DrawPolyMode::Line:
                return "Line";

            default:
                return "";
            }
        }
    }
}