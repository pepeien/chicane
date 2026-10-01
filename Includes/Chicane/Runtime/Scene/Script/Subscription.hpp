#pragma once

#include <cstdint>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME SceneScriptSubscription
    {
    public:
        std::uint64_t token = 0;
        int           ref   = 0;
    };
}
