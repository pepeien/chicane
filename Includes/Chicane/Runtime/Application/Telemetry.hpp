#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Telemetry.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
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