#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Engine
{
    namespace Core
    {
        struct Vertex
        {
        public:
            VkVertexInputBindingDescription getBindingDescription();

        public:
            glm::vec2 pos;
            glm::vec3 color;
        };
    }
}