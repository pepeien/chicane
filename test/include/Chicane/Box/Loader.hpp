#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        CHICANE Audio::Manager* getAudioManager();
        CHICANE Model::Manager* getModelManager();
        CHICANE Texture::Manager* getTextureManager();

        CHICANE const Audio::Instance* loadAudio(const std::string& inFilePath);
        CHICANE const Texture::Instance* loadTexture(const std::string& inFilePath);
        CHICANE const Sky::Instance* loadSky(const std::string& inFilePath);
        CHICANE const Mesh::Instance* loadMesh(const std::string& inFilePath);
    }
}