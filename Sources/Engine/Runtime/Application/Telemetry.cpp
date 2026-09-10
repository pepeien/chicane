#include "Chicane/Runtime/Application/Telemetry.reflected.hpp"

namespace Chicane
{
    ApplicationTelemetry::ApplicationTelemetry()
        : scene({}),
          physics({}),
          animation({}),
          ui({}),
          renderer({})
    {}
}