#pragma once

#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Box/Asset/Texture/Manager.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor.hpp"
#include "Chicane/Runtime/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Renderer.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Texture.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME LSceneCollision : public RendererLayer
        {
        public:
            LSceneCollision();
            ~LSceneCollision();

        public:
            bool onInit() override;
            bool onDestroy() override;
            bool onRebuild() override;
            void onRender(void* outData) override;

        private:
            void loadEvents();

            // Scene
            bool hasMeshes() const;

            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Pipeline
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
            RendererInternals                           m_internals;

            std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

            Descriptor::Bundle                          m_frameDescriptor;

            Scene*                                      m_scene;
            std::vector<CMesh*>                         m_meshes;

            Buffer                                      m_modelVertexBuffer;
            Buffer                                      m_modelIndexBuffer;

            std::vector<vk::ClearValue>                 m_clearValues;
        };
    }
}