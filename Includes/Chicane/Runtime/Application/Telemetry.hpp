#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Telemetry.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    CH_TYPE()
    struct CHICANE_RUNTIME ApplicationTelemetry
    {
    public:
        ApplicationTelemetry();

    public:
        CH_FIELD()
        Telemetry scene;

        CH_FIELD()
        Telemetry ui;

        CH_FIELD()
        Telemetry renderer;
    };
}