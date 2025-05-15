#pragma once

#include "Chicane/Box/Asset/Essential.hpp"
#include "Chicane/Box/Asset/Audio.hpp"
#include "Chicane/Box/Asset/Font.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Mesh.hpp"
#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            static const std::unordered_map<std::string, Type> TYPES = {
                { Audio::EXTENSION,   Type::Audio   },
                { Font::EXTENSION,    Type::Font    },
                { Mesh::EXTENSION,    Type::Mesh    },
                { Model::EXTENSION,   Type::Model   },
                { Sky::EXTENSION,     Type::Sky     },
                { Texture::EXTENSION, Type::Texture }
            };
        }
    }
}