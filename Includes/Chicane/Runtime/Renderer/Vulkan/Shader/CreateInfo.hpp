#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Shader
        {
            struct CHICANE_RUNTIME StageCreateInfo
            {
            public:
                String                  path;
                vk::ShaderStageFlagBits type;
            };
        }
    }
}