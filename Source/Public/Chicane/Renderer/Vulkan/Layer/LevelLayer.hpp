#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level/Instance.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Texture.hpp"
#include "Chicane/Renderer/Vertex.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class LevelLayer : public Layer::Instance
        {
        public:
            LevelLayer();
            ~LevelLayer();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void setup(void* outData) override;
            void render(void* outData) override;

        private:
            void loadEvents();

            void initFrameResources();
            void destroyFrameResources();
            void initTextureResources();
            void destroyTextureResources();
            void initGraphicsPipeline();
            void initFramebuffers();

            // Texture
            void buildTextureData();
            void renderTextures(const vk::CommandBuffer& inCommandBuffer);

            // Model
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
            void buildModelData();
            void destroyModelData();
            void rebuildModelData();
            void renderModels(const vk::CommandBuffer& inCommandBuffer);

            void setupFrames();
            void setFramesAsDirty();

            void updateMeshes(const std::vector<Component*>& inComponents);

        private:
            Vulkan::Renderer::Internals                                         m_internals;

            Level*                                                              m_level;
            std::vector<MeshComponent*>                                         m_meshes;

            std::unique_ptr<Vulkan::GraphicsPipeline::Instance>                 m_graphicsPipeline;

            Vulkan::Descriptor::Bundle                                          m_frameDescriptor;

            Vulkan::Descriptor::Bundle                                          m_textureDescriptor;
            std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_textures;
            Chicane::Texture::Manager*                                          m_textureManager;

            Vulkan::Buffer::Instance                                            m_modelVertexBuffer;
            Vulkan::Buffer::Instance                                            m_modelIndexBuffer;
            Chicane::Model::Manager*                                            m_modelManager;

            std::vector<vk::ClearValue>                                         m_clearValues;
        };
    }
}