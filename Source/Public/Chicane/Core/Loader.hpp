#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Renderer/Model/Manager.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap/Manager.hpp"
#include "Chicane/Renderer/Vulkan/Texture/Manager.hpp"

namespace Chicane
{
    namespace Loader
    {
        Model::Manager* getModelManager();

        Vulkan::CubeMap::Manager* getCubemapManager();
        Vulkan::Texture::Manager* getTextureManager();

        const Box::CubeMap* loadCubemap(const std::string& inFilePath);
        const Box::Texture* loadTexture(const std::string& inFilePath);
        const Box::Mesh* loadMesh(const std::string& inFilePath);

        void reset();
    }
}