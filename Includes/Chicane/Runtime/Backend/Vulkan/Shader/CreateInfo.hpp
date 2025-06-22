#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            struct CHICANE_RUNTIME StageCreateInfo
            {
            public:
                String             path;
                vk::ShaderStageFlagBits type;
            };
        }
    }
}