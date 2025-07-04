#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

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