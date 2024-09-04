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
        CubeMap::Manager* getCubemapManager();
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        Box::Instance loadCubemap(const std::string& inFilePath);
        Box::Instance loadTexture(const std::string& inFilePath);
        Box::Instance loadMesh(const std::string& inFilePath);

        void reset();
    }
}