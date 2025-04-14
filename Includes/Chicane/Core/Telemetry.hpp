#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/FrameTelemetry.hpp"

namespace Chicane
{
    struct Telemetry
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