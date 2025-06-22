#pragma once

#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor.hpp"
#include "Chicane/Runtime/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Renderer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME LShadow : public RendererLayer
        {
        public:
            LShadow();
            ~LShadow();

        public:
            bool onInit() override;
            bool onDestroy() override;
            bool onRebuild() override;
            void onRender(void* outData) override;

        private:
            void loadEvents();

            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Render
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
            RendererInternals                         m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Descriptor::Bundle                          m_frameDescriptor;

            Buffer                                      m_modelVertexBuffer;
            Buffer                                      m_modelIndexBuffer;
            Box::ModelManager*                          m_modelManager;

            std::vector<vk::ClearValue>                 m_clearValues;
        };
    }
}