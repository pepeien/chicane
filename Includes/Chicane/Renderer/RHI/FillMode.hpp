#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class FillMode : std::uint8_t
            {
                Fill,
                Line
            };
        }
    }

    inline String toString(Renderer::RHI::FillMode inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::FillMode::Fill:
            return "Fill";

        case Renderer::RHI::FillMode::Line:
            return "Line";

        default:
            return "";
        }
    }
}
