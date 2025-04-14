#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

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