#pragma once

#include "Box/Asset/Base.hpp"
#include "Box/Asset/Audio.hpp"
#include "Box/Asset/Header.hpp"
#include "Box/Asset/Instance.hpp"
#include "Box/Asset/Mesh.hpp"
#include "Box/Asset/Model.hpp"
#include "Box/Asset/Sky.hpp"
#include "Box/Asset/Texture.hpp"
#include "Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            static const std::unordered_map<std::string, Type> TYPES {
                { Audio::EXTENSION,   Type::Audio   },
                { Mesh::EXTENSION,    Type::Mesh    },
                { Model::EXTENSION,   Type::Model   },
                { Sky::EXTENSION,     Type::Sky     },
                { Texture::EXTENSION, Type::Texture }
            };
        }
    }
}