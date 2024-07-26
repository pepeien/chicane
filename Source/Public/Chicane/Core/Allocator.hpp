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

        void load(const std::string& inFilePath);

        void reset();
    }
}