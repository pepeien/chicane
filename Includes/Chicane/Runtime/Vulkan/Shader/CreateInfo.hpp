#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            struct CHICANE_RUNTIME StageCreateInfo
            {
            public:
                std::string             path;
                vk::ShaderStageFlagBits type;
            };
        }
    }
}