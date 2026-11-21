#include "Chicane/Core/Tracer.hpp"

#if defined(CHICANE_TRACER)

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
    void Tracer::sMarkFrame()
    {
#if defined(CHICANE_TRACER)
        ___tracy_emit_frame_mark(nullptr);
#endif
    }

    void Tracer::sSetThreadName(const char* inName)
    {
#if defined(CHICANE_TRACER)
        if (!inName || inName[0] == '\0')
        {
            return;
        }

        ___tracy_set_thread_name(inName);
#endif
    }

    void Tracer::sSetThreadName(const String& inName)
    {
#if defined(CHICANE_TRACER)
        if (inName.isEmpty())
        {
            return;
        }

        sSetThreadName(inName.toChar());
#endif
    }
}
