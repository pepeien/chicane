#include "Chicane/Core/Timer.hpp"

namespace Chicane
{
    Timer::Timer()
        : m_start(Time::Clock::now()),
          m_end(Time::Clock::now())
    {}

    void Timer::start()
    {
        m_start = Time::Clock::now();
    }

    void Timer::end()
    {
        m_end = Time::Clock::now();

        onTime();
    }

    Time Timer::getDelta() const
    {
        return Time(m_end.point - m_start.point);
    }
}