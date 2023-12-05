#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Render
        {
            struct Vertex
            {
            public:
                static vk::VertexInputBindingDescription getBindingDescription();
                static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();

            public:
                glm::vec2 pos;
                glm::vec3 color;
            };
        }
    }
}