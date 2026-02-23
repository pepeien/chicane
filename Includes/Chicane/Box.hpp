#pragma once

// Macros
#if defined(BUILD_SHARED)
    #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        #if defined(BUILD_EXPORT)
            #define CHICANE_BOX __declspec(dllexport)
        #else
            #define CHICANE_BOX __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define CHICANE_BOX __attribute__((visibility("default")))
    #else
        #define CHICANE_BOX
    #endif
#else
    #define CHICANE_BOX
#endif

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        using AssetObservable   = EventObservable<const class Asset*>;
        using AssetSubscription = EventSubscription<const class Asset*>;

        CHICANE_BOX const class Asset* load(const FileSystem::Path& inFilePath);
        template <typename T>
        inline const T* load(const FileSystem::Path& inFilePath)
        {
            return static_cast<const T*>(load(inFilePath));
        }

        CHICANE_BOX AssetSubscription watchAssets(
            AssetSubscription::NextCallback     inNext,
            AssetSubscription::ErrorCallback    inError    = nullptr,
            AssetSubscription::CompleteCallback inComplete = nullptr
        );
    }
}