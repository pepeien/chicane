#include "Chicane/Core/Telemetry.hpp"

namespace Chicane
{
    Telemetry::Telemetry()
        : Timer(),
          delta(0.0f),
          rate(0U)
    {}

    void Telemetry::onTime()
    {
        delta = getDelta().miliseconds();
        rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
    }
}