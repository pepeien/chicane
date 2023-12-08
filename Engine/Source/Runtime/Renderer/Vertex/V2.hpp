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
                struct V2
                {
                public:
                    static vk::VertexInputBindingDescription getBindingDescription();
                    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();

                public:
                    glm::vec2 position;
                    glm::vec3 color;
                };  
            }
        }
    }
}