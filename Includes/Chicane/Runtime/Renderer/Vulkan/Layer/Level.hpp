#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Runtime/Game/Level/Instance.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE LLevel : public Layer::Instance
        {
        public:
            LLevel();
            ~LLevel();

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

            // Render Pass
            void renderModels(const vk::CommandBuffer& inCommandBuffer);

        private:
            Renderer::Internals                                                 m_internals;

            std::unique_ptr<GraphicsPipeline::Instance>                         m_graphicsPipeline;

            Descriptor::Bundle                                                  m_frameDescriptor;

            Descriptor::Bundle                                                  m_textureDescriptor;
            std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_textures;
            Box::Texture::Manager*                                              m_textureManager;

            Buffer::Instance                                                    m_modelVertexBuffer;
            Buffer::Instance                                                    m_modelIndexBuffer;
            Box::Model::Manager*                                                m_modelManager;

            std::vector<vk::ClearValue>                                         m_clearValues;
        };
    }
}