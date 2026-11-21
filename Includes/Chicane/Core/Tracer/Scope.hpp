#pragma once

#include "Chicane/Core/Tracer/Zone.hpp"

namespace Chicane
{
    struct TracerScope
    {
    public:
        TracerScope(const TracerScope&)            = delete;
        TracerScope& operator=(const TracerScope&) = delete;

        explicit TracerScope(const TracerLocation* inLocation)
            : m_zone(TracerZone::sBegin(inLocation))
        {}

        ~TracerScope() { TracerZone::sEnd(m_zone); }

    private:
        TracerZone m_zone;
    };
}
