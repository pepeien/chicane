#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Timer.hpp"

namespace Chicane
{
    struct CHICANE_CORE Telemetry : public Timer
    {
    public:
        Telemetry();

    protected:
        void onTime() override;

    public:
        float         delta = 0.0f;
        std::uint32_t rate  = 0U;
    };
}