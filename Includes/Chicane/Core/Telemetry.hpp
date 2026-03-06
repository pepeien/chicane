#pragma once

#include <cstdio>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Timer.hpp"

namespace Chicane
{
    CH_TYPE()
    struct CHICANE_CORE Telemetry : public Timer
    {
    public:
        Telemetry();

    protected:
        void onTime() override;

    public:
        CH_FIELD()
        float delta;

        CH_FIELD()
        std::uint32_t rate;
    };
}