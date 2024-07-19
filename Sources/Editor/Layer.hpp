#pragma once

#include "Runtime/Core.hpp"
#include "Runtime/Runtime.hpp"
#include "Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Editor
    {
        class GridLayer : public Layer
        {
        public:
            GridLayer(Window* inWindow);
            ~GridLayer();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void render(
                Frame::Instance& outFrame,
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Extent2D& inSwapChainExtent
            ) override;

        private:
            void initFrameDescriptorSetLayout();
            void initGraphicsPipeline();
            void initFramebuffers();
            void initFrameResources();
            void initVertexBuffers();

        private:
            Renderer* m_renderer;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            vk::DescriptorSet m_descriptorSet;
            Descriptor::Bundle m_frameDescriptor;
            Buffer::Instance m_vertexBuffer;
        };
    }
}