#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    using TimerId = int;

    TimerId CHICANE_CORE setTimeout(std::function<void()> inCallback, std::uint32_t inDelayInMs);
    bool CHICANE_CORE clearTimeout(TimerId inTimerId);

    TimerId CHICANE_CORE setInterval(std::function<void()> inCallback, std::uint32_t inIntervalInMs);
    bool CHICANE_CORE clearInterval(TimerId inTimerId);
}