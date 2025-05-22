#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        CHICANE_BOX Audio::Manager* getAudioManager();
        CHICANE_BOX Font::Manager* getFontManager();
        CHICANE_BOX Model::Manager* getModelManager();
        CHICANE_BOX Texture::Manager* getTextureManager();

        CHICANE_BOX const Asset::Instance* load(const std::string& inFilePath);
        template<typename T>
        const T* load(const std::string& inFilePath)
        {
            return static_cast<const T*>(load(inFilePath));
        }
    }
}