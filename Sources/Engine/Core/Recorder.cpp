#include "Chicane/Core/Recorder.hpp"

namespace Chicane
{
    Recorder::Recorder(float inUpdateRateInMs)
        : Recorder()
    {
        m_cooldownInMs = inUpdateRateInMs;
    }

    Recorder::Recorder()
        : m_begin(Time::Clock::now()),
          m_end(Time::Clock::now()),
          m_cooldownInMs(100.0f) // 0.1 Seconds
    {}

    void Recorder::start()
    {
        m_begin = Time::Clock::now();
    }

    void Recorder::end()
    {
        auto now = Time::Clock::now();

        if (Time::miliseconds(now - m_end.point) < m_cooldownInMs)
        {
            return;
        }

        m_end = now;

        onTime();
    }

    void Recorder::setUpdateRate(float inValue)
    {
        m_cooldownInMs = inValue;
    }
}