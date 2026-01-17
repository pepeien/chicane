#pragma once

#include <cstdio>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Recorder.hpp"

namespace Chicane
{
    struct CHICANE_CORE Telemetry : public Recorder
    {
    public:
        Telemetry();

    protected:
        void onTime() override;

    public:
        float         delta;
        std::uint32_t rate;
    };
}