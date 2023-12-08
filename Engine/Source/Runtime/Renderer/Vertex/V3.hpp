#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                struct V3
                {
                public:
                    static vk::VertexInputBindingDescription getBindingDescription();
                    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();

                public:
                    glm::vec3 position;
                    glm::vec3 color;
                    glm::vec2 textureCoordinates;
                    glm::vec3 normals;
                };  
            }
        }
    }
}