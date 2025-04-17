#pragma once

#include "Core/Essentials.hpp"
#include "Core/FrameTelemetry.hpp"

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