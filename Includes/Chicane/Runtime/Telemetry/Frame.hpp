#pragma once

#include <cstdio>

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Timer.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
    struct CHICANE_RUNTIME FrameTelemetry : public Timer
    {
    public:
        FrameTelemetry();

    protected:
        void onTime() override;

    public:
        CH_FIELD()
        float delta;

        CH_FIELD()
        std::uint32_t rate;
    };
}