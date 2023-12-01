#pragma once

#include "Base.h"

namespace Engine
{
    namespace Core
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