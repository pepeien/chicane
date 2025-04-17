#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Vertex
        {
            CHICANE vk::VertexInputBindingDescription getBindingDescription();
            CHICANE std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
        }
    }
}