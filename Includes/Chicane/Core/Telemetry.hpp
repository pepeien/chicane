#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/FrameTelemetry.hpp"

namespace Chicane
{
    struct CHICANE_CORE Telemetry
    {
    public:
        FrameTelemetry frame {};

    public:
        void startCapture()
        {
            frame.startCapture();
        }

        void endCapture()
        {
            frame.endCapture();
        }
    };
}