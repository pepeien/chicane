#pragma once

#include "Core/Clock.hpp"
#include "Core/Essentials.hpp"

namespace Chicane
{
    struct CHICANE FrameTelemetry
    {
    public:
        void startCapture();
        void endCapture();

    public:
        float         delta    = 0.0f;
        std::uint32_t rate     = 0U;

    private:
        Time          m_begin  = Clock::now();
        Time          m_end    = Clock::now();
        Time          m_update = Clock::now();
    };
}