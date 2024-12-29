#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/GraphicsPipeline/Attachment.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        struct CreateInfo
        {
        public:
            // Modifiers
            bool                                             bHasVertices;
            bool                                             bHasDepth;
            bool                                             bHasBlending;

            // Vertex
            std::string                                      vertexShaderPath;
            std::string                                      fragmentShaderPath;
            vk::VertexInputBindingDescription                bindingDescription;
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

            // Viewport
            vk::Extent2D                                     extent;

            // Atacchment
            Attachment                                       colorAttachment;
            Attachment                                       depthAttachment; // Optional if `hasDepth` == `false`

            // Pipeline Layout
            std::vector<vk::DescriptorSetLayout>             descriptorSetLayouts; // Optional if `hasDepth` == `false`
            std::vector<vk::PushConstantRange>               pushConstantRanges;

            // Vulkan
            vk::Device                                       logicalDevice;
            vk::PolygonMode                                  polygonMode;
        };
    }
}