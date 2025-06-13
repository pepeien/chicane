#include "Chicane/Core/Recorder.hpp"

static inline constexpr const float COOLDOWN_IN_MS = 10.0f; // 0.1 Seconds

namespace Chicane
{
    Recorder::Recorder(float inUpdateRate)
        : Recorder()
    {
        m_cooldown = inUpdateRate;
    }

    Recorder::Recorder()
        : m_begin(Time::Clock::now()),
        m_end(Time::Clock::now()),
        m_cooldown(COOLDOWN_IN_MS)
    {}

    void Recorder::start()
    {
        m_begin = Time::Clock::now();
    }

    void Recorder::end()
    {
        m_end = Time::Clock::now();

        if (Time::miliseconds(m_end.point - m_begin.point) < m_cooldown)
        {
            return;
        }

        onTime();

        m_begin = m_end;
    }

    void Recorder::setUpdateRate(float inValue)
    {
        m_cooldown = inValue;
    }
}