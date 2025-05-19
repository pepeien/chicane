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
                Vec<2, float> size     = {};
                Vec<2, float> position = {};
            };

            struct DrawData
            {
            public:
                const Grid::Component*    component         = {};
                Grid::Primitive           primitive         = {};
                Grid::Primitive::Compiled compiledPrimitive = {};
            };

        public:
            LGrid();
            ~LGrid();

        public:
            bool onBuild() override;
            bool onRebuild() override;
            void onRender(void* outData) override;

        private:
            void loadEvents();

            // Pipeline
            void initGraphicsPipeline();

            // Frame
            void initFramebuffers();

            // Vertex
            void buildVertexBuffer();
            void buildIndexuffer();
            void buildData();
            void destroyData();
            void rebuildData();

            // Render Pass
            void renderComponents(const vk::CommandBuffer& inCommandBuffer);

            // Data
            void refreshDrawDatum();

        private:
            Renderer::Internals                         m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Buffer::Instance                            m_vertexBuffer;
            Buffer::Instance                            m_indexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;

            const Grid::View*                           m_view;
            std::vector<DrawData>                       m_drawDatum;
        };
    }
}