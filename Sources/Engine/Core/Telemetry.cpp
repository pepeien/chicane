#include "Chicane/Core/Telemetry.hpp"

namespace Chicane
{
    Telemetry::Telemetry()
        : Recorder(10.0f),
          delta(0.0f),
          rate(0U)
    {}

    void Telemetry::onTime()
    {
        delta = Time::miliseconds(m_end.point - m_begin.point);
        rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
    }
}