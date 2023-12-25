#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 texturePosition;
        };

        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}