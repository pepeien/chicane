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

        inline String toString(CullingFrontFace inValue)
        {
            switch (inValue)
            {
            case CullingFrontFace::CounterClockwise:
                return "CounterClockwise";

            case CullingFrontFace::Clockwise:
                return "Clockwise";

            default:
                return "";
            }
        }
    }
}