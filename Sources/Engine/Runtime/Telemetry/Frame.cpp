#include "Chicane/Runtime/Telemetry/Frame.reflected.hpp"

namespace Chicane
{
    FrameTelemetry::FrameTelemetry()
        : Timer(),
          delta(0.0f),
          rate(0U)
    {}

    void FrameTelemetry::onTime()
    {
        delta = getDelta().miliseconds();
        rate  = delta > 0.0f ? static_cast<std::uint32_t>(1000.0f / delta) : 0;
    }
}