#pragma once

#include "Chicane/Core/Telemetry.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationTelemetry
    {
    public:
        Telemetry scene    = {};
        Telemetry ui       = {};
        Telemetry renderer = {};
    };
}