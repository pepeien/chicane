#pragma once

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    enum class RendererLayerStatus : std::uint8_t
    {
        Offline,
        Initialized,
        Running
    };
}