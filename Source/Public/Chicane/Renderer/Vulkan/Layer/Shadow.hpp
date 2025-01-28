#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level/Instance.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class LShadow : public Layer::Instance
        {
        public:
            LShadow();
            ~LShadow();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void render(void* outData) override;

        private:
            void loadEvents();

            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Level
            void initGraphicsPipeline();
            void initFramebuffers();

            // Model
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
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
            Buffer::Instance                            m_modelIndexBuffer;
            Box::Model::Manager*                    m_modelManager;

            std::vector<vk::ClearValue>                 m_clearValues;
        };
    }
}