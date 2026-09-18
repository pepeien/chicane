#pragma once

#include <Chicane/Core.hpp>

#if defined(BUILD_SHARED)
    #if IS_WINDOWS
        #if defined(BUILD_EXPORT)
            #define CHICANE_SAMPLE_API __declspec(dllexport)
        #else
            #define CHICANE_SAMPLE_API __declspec(dllimport)
        #endif
    #elif IS_LINUX
        #define CHICANE_SAMPLE_API __attribute__((visibility("default")))
    #else
        #define CHICANE_SAMPLE_API
    #endif
#else
    #define CHICANE_SAMPLE_API
#endif

extern "C" CHICANE_SAMPLE_API bool ChicaneModuleInit();
extern "C" CHICANE_SAMPLE_API void ChicaneModuleShutdown();
