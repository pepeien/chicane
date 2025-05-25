#pragma once

#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME LGrid : public Layer::Instance
        {
        public:
            struct PushConstant
            {
            public:
                Vec2 size     = {};
                Vec2 position = {};
            };

            struct Draw
            {
            public:
                Vec2             size         = Vec2::Zero;
                Vec2             position     = Vec2::Zero;
                std::uint32_t    vertexCount  = 0;
                Buffer::Instance vertexBuffer = {};
                std::uint32_t    indexCount   = 0;
                Buffer::Instance indexBuffer  = {};
            };

        public:
            LGrid();
            ~LGrid();

        public:
            bool onInit() override;
            bool onRebuild() override;
            bool onSetup() override;
            void onRender(void* outData) override;

        private:
            void loadEvents();

            // Pipeline
            void initGraphicsPipeline();

            // Frame
            void initFramebuffers();

            // Render Pass
            void renderComponents(const vk::CommandBuffer& inCommandBuffer);

            // Draw
            void setDrawVertexBuffer(Draw& outDraw, const std::vector<Chicane::Vertex>& inVertices);
            void setDrawIndexBuffer(Draw& outDraw, const std::vector<std::uint32_t>& inIndices);
            void buildDraws(const std::vector<Grid::Component*>& inComponents);
            void destroyDraws();

        private:
            Renderer::Internals                         m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            std::vector<vk::ClearValue>                 m_clearValues;

            const Grid::View*                           m_view;
            std::vector<Draw>                       m_draws;
        };
    }
}