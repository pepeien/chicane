#pragma once

// Macros
#if defined(BUILD_SHARED)
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#if defined(BUILD_EXPORT)
#define CHICANE_SCREECH __declspec(dllexport)
#else
#define CHICANE_SCREECH __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define CHICANE_SCREECH __attribute__((visibility("default")))
#else
#define CHICANE_SCREECH
#endif
#else
#define CHICANE_SCREECH
#endif

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Screech
    {
        CHICANE_SCREECH void* getContext();
        CHICANE_SCREECH void* getEngine();
        CHICANE_SCREECH std::vector<class Device> getDevices();
    }
}