#pragma once

#include "Chicane/Core/Essentials.hpp"

static constexpr float UPDATE_RATE_IN_MS = 1000.0f;

namespace Chicane
{
    struct CHICANE FrameTelemetry
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

            if (std::chrono::duration<float, std::milli>(m_endTime - m_updateTime).count() < UPDATE_RATE_IN_MS)
            {
                return;
            }

            m_updateTime = m_endTime;

            delta = std::chrono::duration<float, std::milli>(m_endTime - m_beginTime).count();
            rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
        }

    public:
        float         delta       = 0.0f;
        std::uint32_t rate        = 0U;

    private:
        Time          m_beginTime  = std::chrono::steady_clock::now();
        Time          m_endTime    = std::chrono::steady_clock::now();
        Time          m_updateTime = std::chrono::steady_clock::now();
    };
}