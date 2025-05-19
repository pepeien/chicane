#include "Chicane/Core/Telemetry.hpp"

static constexpr const float COOLDOWN_IN_MS = 5000.0f; // 5 Seconds

namespace Chicane
{
    Telemetry::Telemetry()
        : Timer(COOLDOWN_IN_MS)
    {}

    void Telemetry::onTime()
    {
        delta = Time::miliseconds(m_end.point - m_begin.point);
        rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
    }
}