#pragma once

#include "Chicane/Box/Asset/Base.hpp"
#include "Chicane/Box/Asset/CubeMap.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Mesh.hpp"
#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            static const std::unordered_map<std::string, Type> TYPES {
                { CubeMap::EXTENSION, Type::CubeMap },
                { Mesh::EXTENSION,    Type::Mesh    },
                { Model::EXTENSION,   Type::Model   },
                { Texture::EXTENSION, Type::Texture }
            };
        }
    }
}