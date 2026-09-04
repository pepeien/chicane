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

#include "Chicane/Box/Asset/Load.hpp"

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        class AssetPreview;

        using AssetObservable     = EventObservable<const class Asset*>;
        using AssetSubscription   = EventSubscription<const class Asset*>;
        using PreviewObservable   = EventObservable<const AssetPreview*>;
        using PreviewSubscription = EventSubscription<const AssetPreview*>;

        CHICANE_BOX std::vector<const class Asset*> getById(const String& inId);
        template <typename T>
        inline const T* getById(const String& inId)
        {
            for (auto asset : getById(inId))
            {
                if (const T* result = dynamic_cast<const T*>(asset))
                {
                    return result;
                }
            }

            return nullptr;
        }

        CHICANE_BOX const class Font* findFont(const String& inFamily, float inWeight);

        CHICANE_BOX void notify(const class Asset* inAsset);

        CHICANE_BOX const class Asset* load(const FileSystem::Path& inFilePath);
        template <typename T>
        inline const T* load(const FileSystem::Path& inFilePath)
        {
            return static_cast<const T*>(load(inFilePath));
        }

        CHICANE_BOX const AssetPreview* findPreview(const FileSystem::Path& inFilePath);
        CHICANE_BOX const AssetPreview* loadPreview(const FileSystem::Path& inFilePath);
        CHICANE_BOX bool embedPreview(const FileSystem::Path& inFilePath);
        CHICANE_BOX void requestPreview(const FileSystem::Path& inFilePath);
        CHICANE_BOX void pumpPreview();

        CHICANE_BOX AssetSubscription watch(
            AssetSubscription::NextCallback     inNext,
            AssetSubscription::ErrorCallback    inError    = nullptr,
            AssetSubscription::CompleteCallback inComplete = nullptr
        );
        CHICANE_BOX PreviewSubscription watchPreview(
            PreviewSubscription::NextCallback     inNext,
            PreviewSubscription::ErrorCallback    inError    = nullptr,
            PreviewSubscription::CompleteCallback inComplete = nullptr
        );
    }
}