#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Telemetry/Frame.hpp"

namespace Chicane
{
    CH_TYPE()
    struct CHICANE_RUNTIME Telemetry
    {
    public:
        Telemetry();

    public:
        void start();

        void end();

    public:
        CH_FIELD()
        FrameTelemetry frame;
    };
}