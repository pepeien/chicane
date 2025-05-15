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

        CHICANE_BOX const Audio::Instance* loadAudio(const std::string& inFilePath);
        CHICANE_BOX const Font::Instance* loadFont(const std::string& inFilePath);
        CHICANE_BOX const Texture::Instance* loadTexture(const std::string& inFilePath);
        CHICANE_BOX const Sky::Instance* loadSky(const std::string& inFilePath);
        CHICANE_BOX const Mesh::Instance* loadMesh(const std::string& inFilePath);
    }
}