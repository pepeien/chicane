#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            struct StageCreateInfo
            {
            public:
                std::string             path;
                vk::ShaderStageFlagBits type;
            };
        }
    }
}