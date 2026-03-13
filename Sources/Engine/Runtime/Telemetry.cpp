#include "Chicane/Runtime/Telemetry.reflected.hpp"

namespace Chicane
{
    Telemetry::Telemetry()
        : frame({})
    {}

    void Telemetry::start()
    {
        frame.start();
    }

    void Telemetry::end()
    {
        frame.end();
    }
}