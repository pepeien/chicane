#include "Chicane/Core/Timer.hpp"

static constexpr const float COOLDOWN_IN_MS = 10.0f; // 0.1 Seconds

namespace Chicane
{
    Timer::Timer(float inUpdateRate)
        : Timer()
    {
        m_cooldown = inUpdateRate;
    }

    Timer::Timer()
        : m_begin(Clock::now()),
        m_end(Clock::now()),
        m_time(Clock::now()),
        m_cooldown(COOLDOWN_IN_MS)
    {}

    void Timer::start()
    {
        m_begin = Clock::now();
    }

    void Timer::end()
    {
        m_end = Clock::now();

        if (Time::miliseconds(m_end.point - m_time.point) < m_cooldown)
        {
            return;
        }

        m_time = m_end;

        onTime();
    }

    void Timer::setUpdateRate(float inValue)
    {
        m_cooldown = inValue;
    }
}