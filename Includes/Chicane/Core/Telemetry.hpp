#pragma once

#include "Chicane/Core/Essentials.hpp"
#include "Chicane/Core/FrameTelemetry.hpp"

namespace Chicane
{
    struct CHICANE Telemetry
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