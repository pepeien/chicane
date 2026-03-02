#pragma once

#include <vector>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Shader/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Shader
        {
        public:
            using List = std::vector<Shader>;

        public:
            FileSystem::Path source = "";
            ShaderType       type   = ShaderType::Undefined;
        };
    }
}