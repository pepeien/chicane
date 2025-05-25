#include "Chicane/Core/Telemetry.hpp"

static constexpr inline const float COOLDOWN_IN_MS = 5000.0f; // 5 Seconds

namespace Chicane
{
    Telemetry::Telemetry()
        : Recorder(COOLDOWN_IN_MS),
        delta(0.0f),
        rate(0U)
    {}

    void Telemetry::onTime()
    {
        delta = Time::miliseconds(m_end.point - m_begin.point);
        rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
    }
}