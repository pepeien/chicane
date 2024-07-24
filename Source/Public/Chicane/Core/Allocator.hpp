#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Renderer/CubeMap/Manager.hpp"
#include "Chicane/Renderer/Model/Manager.hpp"
#include "Chicane/Renderer/Texture/Manager.hpp"

namespace Chicane
{
    namespace Allocator
    {
        CubeMap::Manager* getCubemapManager();
        Model::Manager* getModelManager();
        Texture::Manager* getTextureManager();

        void loadMesh(const Box::Instance& inInstance);
        void loadModel(const Box::Instance& inInstance);
        void loadTexture(const Box::Instance& inInstance);

        void load(const std::string& inFilePath);
        void load(const Box::Instance& inInstance);

        void reset();
    }
}