#pragma once

#include "Chicane/Grid/Component/View.hpp"
#include "Chicane/Runtime/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Renderer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME LGrid : public RendererLayer
        {
        public:
            struct PushConstant
            {
            public:
                Vec2  size     = Vec2::Zero;
                Vec2  position = Vec2::Zero;
                float zIndex   = 0.0f;
            };

            struct Draw
            {
            public:
                Grid::Component* component    = nullptr;
                std::uint32_t    vertexCount  = 0;
                Buffer           vertexBuffer = {};
                std::uint32_t    indexCount   = 0;
                Buffer           indexBuffer  = {};

                bool             bIsOutdated  = true;
                bool             bIsDrawable  = false;
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
            void destroyDrawVertexBuffer(Draw& outDraw);
            void setDrawIndexBuffer(Draw& outDraw, const std::vector<std::uint32_t>& inIndices);
            void destroyDrawIndexBuffer(Draw& outDraw);
            bool hasDraw(Grid::Component* inComponent);
            Draw& getDraw(Grid::Component* inComponent);
            void buildDraw(Grid::Component* inComponent);
            void refreshDraw(Grid::Component* inComponent);
            void destroyDraws();
            void destroyDraw(Draw& outDraw);

        private:
            RendererInternals                           m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            std::vector<vk::ClearValue>                 m_clearValues;

            Grid::View*                                 m_view;
            std::vector<Draw>                           m_draws;
        };
    }
}