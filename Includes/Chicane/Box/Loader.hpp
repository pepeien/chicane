#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        CHICANE_BOX Sound::Manager* getSoundManager();
        CHICANE_BOX Font::Manager* getFontManager();
        CHICANE_BOX Model::Manager* getModelManager();
        CHICANE_BOX Texture::Manager* getTextureManager();

        CHICANE_BOX const Asset::Instance* load(const FileSystem::Path& inFilePath);
        template<typename T>
        const T* load(const FileSystem::Path& inFilePath)
        {
            return static_cast<const T*>(load(inFilePath));
        }
    }
}