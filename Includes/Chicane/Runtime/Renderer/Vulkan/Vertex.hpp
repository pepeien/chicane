#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

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