#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class ListPushStrategy : std::uint8_t
    {
        Front,
        Back,
        Before, // Require to have the `Anchor` `typename`
        After   // Require to have the `Anchor` `typename`
    };
}