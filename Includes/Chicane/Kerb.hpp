#pragma once

// Macros
#if defined(BUILD_SHARED)
    #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        #if defined(BUILD_EXPORT)
            #define CHICANE_KERB __declspec(dllexport)
        #else
            #define CHICANE_KERB __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define CHICANE_KERB __attribute__((visibility("default")))
    #else
        #define CHICANE_KERB
    #endif
#else
    #define CHICANE_KERB
#endif

#include <Jolt/Jolt.h>

namespace Chicane
{
    namespace Kerb
    {
        CHICANE_KERB void init();
        CHICANE_KERB void exit();
    }
}