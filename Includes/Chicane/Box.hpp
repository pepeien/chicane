#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Asset/Font/Manager.hpp"
#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Box/Asset/Sound/Manager.hpp"
#include "Chicane/Box/Asset/Texture/Manager.hpp"
#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        CHICANE_BOX SoundManager* getSoundManager();
        CHICANE_BOX FontManager* getFontManager();
        CHICANE_BOX ModelManager* getModelManager();
        CHICANE_BOX TextureManager* getTextureManager();

        CHICANE_BOX const Asset* load(const FileSystem::Path& inFilePath);
        template<typename T>
        const T* load(const FileSystem::Path& inFilePath)
        {
            return static_cast<const T*>(load(inFilePath));
        }
    }
}