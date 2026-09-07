#include "Chicane/Runtime/Application/Telemetry.reflected.hpp"

namespace Chicane
{
    ApplicationTelemetry::ApplicationTelemetry()
        : scene({}),
          physics({}),
          ui({}),
          renderer({})
    {}
}