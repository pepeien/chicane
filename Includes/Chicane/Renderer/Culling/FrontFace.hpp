#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class CullingFrontFace : std::uint8_t
        {
            CounterClockwise,
            Clockwise
        };
    }

    inline String toString(Renderer::CullingFrontFace inValue)
    {
        switch (inValue)
        {
        case Renderer::CullingFrontFace::CounterClockwise:
            return "CounterClockwise";

        case Renderer::CullingFrontFace::Clockwise:
            return "Clockwise";

        default:
            return "";
        }
    }
}
