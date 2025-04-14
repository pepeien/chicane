#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Vertex
        {
            vk::VertexInputBindingDescription getBindingDescription();
            std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
        }
    }
}