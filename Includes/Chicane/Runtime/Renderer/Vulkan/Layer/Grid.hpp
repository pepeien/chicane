#pragma once

#include "Core.hpp"
#include "Grid/Component/Instance.hpp"
#include "Runtime/Renderer/Layer.hpp"
#include "Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE LGrid : public Layer::Instance
        {
        public:
            LGrid();
            ~LGrid();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;
    
            void setup(void* outData) override;
            void render(void* outData) override;

        private:
            void loadEvents();

            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Pipeline
            void initGraphicsPipeline();
            void initFramebuffers();

            // Model
            void buildModelVertexBuffer();
            void buildModelData();
            void destroyModelData();
            void rebuildModelData();

            // Render Pass
            void renderModels(const vk::CommandBuffer& inCommandBuffer);

        private:
            Renderer::Internals                         m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Descriptor::Bundle                          m_frameDescriptor;

            Buffer::Instance                            m_modelVertexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;

            std::vector<const Grid::Component*>         m_components;
        };
    }
}