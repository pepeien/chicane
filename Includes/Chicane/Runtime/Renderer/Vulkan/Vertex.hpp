#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Vertex
        {
            CHICANE_RUNTIME vk::VertexInputBindingDescription getBindingDescription();
            CHICANE_RUNTIME std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
        }
    }
}