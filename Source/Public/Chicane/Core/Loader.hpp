#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Renderer/Audio/Manager.hpp"
#include "Chicane/Renderer/CubeMap/Manager.hpp"
#include "Chicane/Renderer/Model/Manager.hpp"
#include "Chicane/Renderer/Texture/Manager.hpp"

namespace Chicane
{
    namespace Loader
    {
        Audio::Manager* getAudioManager();
        CubeMap::Manager* getCubeMapManager();
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        const Box::Audio* loadAudio(const std::string& inFilePath);
        const Box::CubeMap* loadCubeMap(const std::string& inFilePath);
        const Box::Texture* loadTexture(const std::string& inFilePath);
        const Box::Mesh* loadMesh(const std::string& inFilePath);

        void reset();
    }
}