#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
            glm::vec2 position;
            glm::vec3 color;
            glm::vec2 texturePosition;
        };

        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}