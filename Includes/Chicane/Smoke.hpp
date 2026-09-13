#pragma once

// Macros
#if defined(BUILD_SHARED)
    #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        #if defined(BUILD_EXPORT)
            #define CHICANE_SMOKE __declspec(dllexport)
        #else
            #define CHICANE_SMOKE __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define CHICANE_SMOKE __attribute__((visibility("default")))
    #else
        #define CHICANE_SMOKE
    #endif
#else
    #define CHICANE_SMOKE
#endif

namespace Chicane
{
    namespace Smoke
    {
        CHICANE_SMOKE void init();
        CHICANE_SMOKE void shutdown();
        CHICANE_SMOKE void tick(float inDeltaTime);
    }
}
