#pragma once

#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Runtime/Vulkan/Renderer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME LSky : public Layer::Instance
        {
        public:
            LSky();
            ~LSky();

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

            void initTextureResources();
            void destroyTextureResources();

            // Pipeline
            void initGraphicsPipeline();
            void initFramebuffers();

            // Texture
            void buildTextureData();

            // Model
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
            void buildModelData();
            void destroyModelData();
            void rebuildModelData();

        private:
            Renderer::Internals                         m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Descriptor::Bundle                          m_frameDescriptor;
            Descriptor::Bundle                          m_textureDescriptor;

            std::unique_ptr<Sky::Instance>              m_sky;
            const Box::Sky*                             m_asset;

            Buffer::Instance                            m_modelVertexBuffer;
            Buffer::Instance                            m_modelIndexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;

            Box::ModelManager*                          m_modelManager;
            Box::TextureManager*                        m_textureManager;
        };
    }
}