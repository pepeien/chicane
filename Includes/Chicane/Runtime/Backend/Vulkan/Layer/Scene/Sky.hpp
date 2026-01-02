#pragma once

#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Box/Asset/Texture/Manager.hpp"
#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor.hpp"
#include "Chicane/Runtime/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Renderer.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Sky.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME LSceneSky : public RendererLayer
        {
        public:
            LSceneSky();
            ~LSceneSky();

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
            RendererInternals                           m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Descriptor::Bundle                          m_frameDescriptor;
            Descriptor::Bundle                          m_textureDescriptor;

            std::unique_ptr<Sky::Instance>              m_sky;
            const Box::Sky*                             m_asset;

            Buffer                                      m_modelVertexBuffer;
            Buffer                                      m_modelIndexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;

            Box::ModelManager*                          m_modelManager;
            Box::TextureManager*                        m_textureManager;
        };
    }
}