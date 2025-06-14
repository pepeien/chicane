#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Editor
    {
        class LGrid : public Chicane::Layer::Instance
        {
        public:
            LGrid();
            ~LGrid();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void render(void* outData) override;

        private:
            void initFrameDescriptorSetLayout();
            std::vector<Vulkan::Shader::StageCreateInfo> getGraphicsPipelineShaders();
            std::vector<vk::DescriptorSetLayout> getGraphicsPipelineDescriptorLayouts();
            std::vector<Vulkan::GraphicsPipeline::Attachment> getGraphicsPipelineAttachments();
            void initGraphicsPipeline();
            void initFramebuffers();
            void initFrameResources();
            void initVertexBuffers();

        private:
            Vulkan::Renderer::Internals                         m_internals;

            std::unique_ptr<Vulkan::GraphicsPipeline::Instance> m_graphicsPipeline;

            Vulkan::Descriptor::Bundle                          m_frameDescriptor;
            Vulkan::Buffer::Instance                            m_vertexBuffer;

            std::vector<vk::ClearValue>                         m_clearValues;
        };
    }
}