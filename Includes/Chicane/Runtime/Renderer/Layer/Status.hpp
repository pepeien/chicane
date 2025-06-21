#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    enum class RendererLayerStatus : std::uint8_t
    {
        Offline,
        Initialized,
        Running
    };
}