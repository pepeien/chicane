#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Tracer/Scope.hpp"

namespace Chicane
{
    class CHICANE_CORE Tracer
    {
    public:
        static void sMarkFrame();
        static void sSetThreadName(const char* inName);
        static void sSetThreadName(const String& inName);
    };
}

#if defined(CHICANE_TRACER)

    #define CH_TRACER_CONCAT_INNER(inLeft, inRight) inLeft##inRight
    #define CH_TRACER_CONCAT(inLeft, inRight) CH_TRACER_CONCAT_INNER(inLeft, inRight)

    #define CH_TRACER_SCOPE()                                                                                          \
        static const ::Chicane::TracerLocation CH_TRACER_CONCAT(                                                       \
            _tracerLocation,                                                                                           \
            __LINE__                                                                                                   \
        ) = {nullptr, __FUNCTION__, __FILE__, static_cast<std::uint32_t>(__LINE__), 0u};                               \
        [[maybe_unused]] ::Chicane::TracerScope CH_TRACER_CONCAT(_tracerScope, __LINE__)(                              \
            &CH_TRACER_CONCAT(_tracerLocation, __LINE__)                                                               \
        )

    #define CH_TRACER_SCOPE_NAMED(inName)                                                                              \
        static const ::Chicane::TracerLocation CH_TRACER_CONCAT(                                                       \
            _tracerLocation,                                                                                           \
            __LINE__                                                                                                   \
        ) = {(inName), __FUNCTION__, __FILE__, static_cast<std::uint32_t>(__LINE__), 0u};                              \
        [[maybe_unused]] ::Chicane::TracerScope CH_TRACER_CONCAT(_tracerScope, __LINE__)(                              \
            &CH_TRACER_CONCAT(_tracerLocation, __LINE__)                                                               \
        )

#else

    #define CH_TRACER_SCOPE()
    #define CH_TRACER_SCOPE_NAMED(inName)

#endif
