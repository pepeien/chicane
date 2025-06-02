#pragma once

#include "Chicane/Box/Asset/Font.hpp"
#include "Chicane/Box/Asset/Header.hpp"
#include "Chicane/Box/Asset/Instance.hpp"
#include "Chicane/Box/Asset/Mesh.hpp"
#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Box/Asset/Sound.hpp"
#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Box/Asset/Type.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            // Settings
            static inline constexpr const std::uint32_t CURRENT_VERSION = 1;

            // Attributes
            static inline constexpr const char* VERSION_ATTRIBUTE_NAME = "version";
            static inline constexpr const char* ID_ATTRIBUTE_NAME      = "id";
        }
    }
}