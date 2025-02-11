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

        const Box::Audio::Instance* loadAudio(const std::string& inFilePath);
        const Box::Texture::Instance* loadTexture(const std::string& inFilePath);
        const Box::Sky::Instance* loadSky(const std::string& inFilePath);
        const Box::Mesh::Instance* loadMesh(const std::string& inFilePath);
    }
}