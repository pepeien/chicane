#pragma once

// Macros
#if defined(BUILD_SHARED)
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#      if defined(BUILD_EXPORT)
#          define CHICANE_BOX __declspec(dllexport)
#      else
#          define CHICANE_BOX __declspec(dllimport)
#      endif
#   elif defined(__GNUC__) || defined(__clang__)
#       define CHICANE_BOX __attribute__((visibility("default")))
#   else
#       define CHICANE_BOX
#   endif
#else
#   define CHICANE_BOX
#endif

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        CHICANE_BOX class SoundManager* getSoundManager();
        CHICANE_BOX class FontManager* getFontManager();
        CHICANE_BOX class ModelManager* getModelManager();
        CHICANE_BOX class TextureManager* getTextureManager();

        CHICANE_BOX const class Asset* load(const FileSystem::Path& inFilePath);
        template<typename T>
        const T* load(const FileSystem::Path& inFilePath)
        {
            return static_cast<const T*>(load(inFilePath));
        }
    }
}