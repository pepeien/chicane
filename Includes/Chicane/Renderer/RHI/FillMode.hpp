#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(Renderer::RHI::FillMode inValue);
}
