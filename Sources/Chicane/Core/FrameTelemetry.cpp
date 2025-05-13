#include "Chicane/Core/FrameTelemetry.hpp"

static constexpr float UPDATE_RATE_IN_MS = 5000.0f;

namespace Chicane
{
    void FrameTelemetry::startCapture()
    {
        m_begin = Clock::now();
    }

    void FrameTelemetry::endCapture()
    {
        m_end = Clock::now();

        if (Time::miliseconds(m_end.point - m_update.point) < UPDATE_RATE_IN_MS)
        {
            return;
        }

        m_update = m_end;

        delta = Time::miliseconds(m_end.point - m_begin.point);
        rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
    }
}