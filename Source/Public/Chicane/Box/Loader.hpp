#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        Audio::Manager* getAudioManager();
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        const Audio::Instance* loadAudio(const std::string& inFilePath);
        const Texture::Instance* loadTexture(const std::string& inFilePath);
        const Sky::Instance* loadSky(const std::string& inFilePath);
        const Mesh::Instance* loadMesh(const std::string& inFilePath);
    }
}