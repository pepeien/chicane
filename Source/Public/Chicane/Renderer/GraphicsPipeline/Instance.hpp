#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/GraphicsPipeline/CreateInfo.hpp"

namespace Chicane
{
    namespace GraphicsPipeline
    {
        class Instance
        {
        public:
            Instance(const CreateInfo& inCreateInfo);
            ~Instance();

        public:
            vk::PipelineLayout                               layout;
            vk::RenderPass                                   renderPass;
            vk::Pipeline                                     instance;

        private:
            // Modifiers
            bool                                             m_bHasVertices;
            bool                                             m_bHasDepth;

            // Vertex
            std::string                                      m_vertexShaderPath;
            std::string                                      m_fragmentShaderPath;
            vk::VertexInputBindingDescription                m_bindingDescription;
            std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;

            // Viewport
            vk::Extent2D                                     m_extent;

            // Layout
            std::vector<vk::DescriptorSetLayout>             m_descriptorSetLayouts;
            std::vector<vk::PushConstantRange>               m_pushConstantRanges;

            // Vulkan
            vk::Device                                       m_logicalDevice;
        };
    }
}