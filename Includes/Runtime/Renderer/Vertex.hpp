#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 texturePosition;
            glm::vec3 normal;
        };

        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}