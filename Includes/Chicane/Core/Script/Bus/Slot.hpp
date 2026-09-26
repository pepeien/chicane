#pragma once

#include <cstdint>
#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Script
    {
        struct CHICANE_CORE Slot
        {
        public:
            std::uint64_t                      token = 0U;
            std::function<void(const String&)> callback;
        };
    }
}
