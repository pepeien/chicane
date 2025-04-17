#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            struct CHICANE StageCreateInfo
            {
            public:
                std::string             path;
                vk::ShaderStageFlagBits type;
            };
        }
    }
}