#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Vertex/Instance.hpp"

namespace Chicane
{
    namespace Vertex
    {
        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}