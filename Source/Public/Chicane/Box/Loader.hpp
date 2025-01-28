#pragma once

#include "Chicane/Box/Asset.hpp"
#include "Chicane/Box/Manager.hpp"

namespace Chicane
{
    namespace Box
    {
        Audio::Manager* getAudioManager();
        CubeMap::Manager* getCubeMapManager();
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        const Box::Audio::Instance* loadAudio(const std::string& inFilePath);
        const Box::CubeMap::Instance* loadCubeMap(const std::string& inFilePath);
        const Box::Texture::Instance* loadTexture(const std::string& inFilePath);
        const Box::Mesh::Instance* loadMesh(const std::string& inFilePath);

        void reset();
    }
}