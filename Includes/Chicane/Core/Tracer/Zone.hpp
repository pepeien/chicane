#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Tracer/Location.hpp"

namespace Chicane
{
    struct CHICANE_CORE TracerZone
    {
    public:
        static TracerZone sBegin(const TracerLocation* inLocation);

        static void sEnd(TracerZone inZone);

    public:
        std::uint32_t id;
        std::int32_t  active;
        std::uint64_t connectionId;
    };
}
