#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Renderer/Model/Manager.hpp"
#include "Chicane/Renderer/Texture/Manager.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap/Manager.hpp"

namespace Chicane
{
    namespace Loader
    {
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        Vulkan::CubeMap::Manager* getCubemapManager();

        const Box::CubeMap* loadCubemap(const std::string& inFilePath);
        const Box::Texture* loadTexture(const std::string& inFilePath);
        const Box::Mesh* loadMesh(const std::string& inFilePath);

        void reset();
    }
}