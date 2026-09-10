#pragma once

// Macros
#if defined(BUILD_SHARED)
    #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        #if defined(BUILD_EXPORT)
            #define CHICANE_DRIFT __declspec(dllexport)
        #else
            #define CHICANE_DRIFT __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define CHICANE_DRIFT __attribute__((visibility("default")))
    #else
        #define CHICANE_DRIFT
    #endif
#else
    #define CHICANE_DRIFT
#endif

namespace Chicane
{
    class Transformable;
    struct String;

    namespace Drift
    {
        struct Clip;

        CHICANE_DRIFT void init();
        CHICANE_DRIFT void shutdown();
        CHICANE_DRIFT void tick(float inDeltaTime);

        CHICANE_DRIFT void add(const Clip& inClip);
        CHICANE_DRIFT void remove(const String& inName);
        CHICANE_DRIFT bool has(const String& inName);

        CHICANE_DRIFT void play(Transformable& inTarget, const String& inClip);
        CHICANE_DRIFT void enqueue(Transformable& inTarget, const String& inClip);
        CHICANE_DRIFT void stop(Transformable& inTarget);
        CHICANE_DRIFT void unbind(Transformable& inTarget);

        CHICANE_DRIFT bool isPlaying(const Transformable& inTarget);
    }
}