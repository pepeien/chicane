#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanVertex
        {
            CHICANE_RENDERER vk::VertexInputBindingDescription getBindingDescription();
            CHICANE_RENDERER std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
        }
    }
}