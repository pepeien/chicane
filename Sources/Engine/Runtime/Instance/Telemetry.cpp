#include "Chicane/Runtime/Instance/Telemetry.reflected.hpp"

namespace Chicane
{
    InstanceTelemetry::InstanceTelemetry()
        : scene({}),
          physics({}),
          animation({}),
          ui({}),
          renderer({})
    {}
}