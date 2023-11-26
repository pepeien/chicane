#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

public:
    VkVertexInputBindingDescription getBindingDescription();
};