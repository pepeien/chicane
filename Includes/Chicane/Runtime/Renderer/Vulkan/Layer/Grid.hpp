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

        public:
            LGrid();
            ~LGrid();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void render(void* outData) override;

        private:
            void loadEvents();

            // Pipeline
            void initGraphicsPipeline();

            // Frame
            void initFramebuffers();

            // Vertex
            void buildVertexBuffer();
            void buildData();
            void destroyData();
            void rebuildData();

            // Render Pass
            void renderComponents(const vk::CommandBuffer& inCommandBuffer);

        private:
            Renderer::Internals                         m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Buffer::Instance                            m_vertexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;

            const Grid::View*                           m_view;
            std::vector<const Grid::Component*>         m_components;
        };
    }
}