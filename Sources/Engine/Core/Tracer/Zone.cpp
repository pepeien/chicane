#include "Chicane/Core/Tracer/Zone.hpp"

#if defined(CHICANE_TRACER)

    #include <cstddef>
    #include <cstring>

    #if !defined(NOMINMAX)
        #define NOMINMAX
    #endif

    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif

    #include <tracy/TracyC.h>

#endif

namespace Chicane
{
#if defined(CHICANE_TRACER)

    static_assert(sizeof(TracerLocation) == sizeof(___tracy_source_location_data));
    static_assert(alignof(TracerLocation) == alignof(___tracy_source_location_data));
    static_assert(offsetof(TracerLocation, name) == offsetof(___tracy_source_location_data, name));
    static_assert(offsetof(TracerLocation, function) == offsetof(___tracy_source_location_data, function));
    static_assert(offsetof(TracerLocation, file) == offsetof(___tracy_source_location_data, file));
    static_assert(offsetof(TracerLocation, line) == offsetof(___tracy_source_location_data, line));
    static_assert(offsetof(TracerLocation, color) == offsetof(___tracy_source_location_data, color));

    static_assert(sizeof(TracerZone) == sizeof(___tracy_c_zone_context));
    static_assert(alignof(TracerZone) == alignof(___tracy_c_zone_context));
    static_assert(offsetof(TracerZone, id) == offsetof(___tracy_c_zone_context, id));
    static_assert(offsetof(TracerZone, active) == offsetof(___tracy_c_zone_context, active));
    static_assert(offsetof(TracerZone, connectionId) == offsetof(___tracy_c_zone_context, connectionId));

#endif

    TracerZone TracerZone::sBegin(const TracerLocation* inLocation)
    {
        TracerZone zone = {};

#if defined(CHICANE_TRACER)
        if (!inLocation)
        {
            return zone;
        }

        const ___tracy_c_zone_context context = ___tracy_emit_zone_begin(
            reinterpret_cast<const ___tracy_source_location_data*>(inLocation),
            1
        );

        if (!context.active)
        {
            return zone;
        }

        std::memcpy(&zone, &context, sizeof(zone));
#else
        (void)inLocation;
#endif

        return zone;
    }

    void TracerZone::sEnd(TracerZone inZone)
    {
#if defined(CHICANE_TRACER)
        ___tracy_c_zone_context context = {};
        std::memcpy(&context, &inZone, sizeof(context));

        ___tracy_emit_zone_end(context);
#else
        (void)inZone;
#endif
    }
}
