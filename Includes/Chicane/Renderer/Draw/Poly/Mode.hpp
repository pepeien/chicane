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
    }

    inline String toString(Renderer::DrawPolyMode inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPolyMode::Fill:
            return "Fill";

        case Renderer::DrawPolyMode::Line:
            return "Line";

        default:
            return "";
        }
    }
}
