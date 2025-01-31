#pragma once

#include "Chicane/Base.hpp"
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