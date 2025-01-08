#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Renderer/CubeMap/Manager.hpp"
#include "Chicane/Renderer/Model/Manager.hpp"
#include "Chicane/Renderer/Texture/Manager.hpp"

namespace Chicane
{
    namespace Loader
    {
        CubeMap::Manager* getCubeMapManager();
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        const Box::CubeMap* loadCubemap(const std::string& inFilePath);
        const Box::Texture* loadTexture(const std::string& inFilePath);
        const Box::Mesh* loadMesh(const std::string& inFilePath);

        void reset();
    }
}