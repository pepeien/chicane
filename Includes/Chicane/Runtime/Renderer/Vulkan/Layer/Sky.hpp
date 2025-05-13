#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE LSky : public Layer::Instance
        {
        public:
            LSky();
            ~LSky();

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
            const Box::Sky::Instance*                   m_asset;

            Buffer::Instance                            m_modelVertexBuffer;
            Buffer::Instance                            m_modelIndexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;
        };
    }
}