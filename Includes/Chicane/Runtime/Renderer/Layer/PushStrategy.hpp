#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    enum class RendererLayerPushStrategy : std::uint8_t
    {
        Front,
        Back,
        BeforeLayer, // Require to have the ref layer `ID`
        AfterLayer   // Require to have the ref layer `ID`
    };
}