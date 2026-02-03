#pragma once

// Macros
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define IS_WINDOWS true
#else
    #define IS_WINDOWS false
#endif

#if defined(__linux__) || (defined(__GNUC__) || defined(__clang__))
    #define IS_LINUX true
#else
    #define IS_LINUX false
#endif

#if defined(NDEBUG)
    #define IS_DEBUGGING false
#else
    #define IS_DEBUGGING true
#endif

#if defined(BUILD_SHARED)
    #if IS_WINDOWS
        #if defined(BUILD_EXPORT)
            #define CHICANE_CORE __declspec(dllexport)
        #else
            #define CHICANE_CORE __declspec(dllimport)
        #endif
    #elif IS_LINUX
        #define CHICANE_CORE __attribute__((visibility("default")))
    #else
        #define CHICANE_CORE
    #endif
#else
    #define CHICANE_CORE
#endif

#if defined(BUILD_OPENGL)
    #define CHICANE_OPENGL true
#else
    #define CHICANE_OPENGL false
#endif

#if defined(BUILD_VULKAN)
    #define CHICANE_VULKAN true
#else
    #define CHICANE_VULKAN false
#endif