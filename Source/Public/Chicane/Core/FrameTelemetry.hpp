#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    struct FrameTelemetry
    {
    public:
        typedef std::chrono::time_point<std::chrono::steady_clock> Time;

    public:
        void startCapture()
        {
            m_beginTime = std::chrono::steady_clock::now();
        }

        void endCapture()
        {
            m_endTime = std::chrono::steady_clock::now();

            delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(
                m_endTime - m_beginTime
            ).count();
            rate = 1000U / delta;
        }

    public:
        float         delta = 0.0f;
        std::uint32_t rate  = 0U;

    private:
        Time m_beginTime    = std::chrono::steady_clock::now();
        Time m_endTime      = std::chrono::steady_clock::now();
    };
}