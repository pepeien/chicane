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

    public:
        inline operator String() const { return toString(); }

    protected:
        void onTime() override;

    public:
        void set(float inDelta);

        String toString() const;

    public:
        CH_FIELD()
        float delta;

        CH_FIELD()
        std::uint32_t rate;
    };
}