#pragma once

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class LayerStatus : std::uint8_t
        {
            Offline,
            Initialized,
            Running
        };
    }
}