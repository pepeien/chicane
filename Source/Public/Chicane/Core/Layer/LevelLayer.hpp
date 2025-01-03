#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class LevelLayer : public Layer
    {
    public:
        LevelLayer(Window::Instance* inWindow);
        ~LevelLayer();

    public:
        bool canRender() const override;

        void build() override;
        void destroy() override;
        void rebuild() override;

        void setup(Frame::Instance& outFrame) override;
        void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void loadEvents();

        void initFrameResources();
        void destroyFrameResources();
        void initTextureResources();
        void destroyTextureResources();
        void initGraphicsPipeline();
        void initFramebuffers();
        void buildTextures();
        void buildModelData();
        void destroyModelData();
        void rebuildModelData();

        void setupFrames();
        void setFramesAsDirty();

        void updateMeshes(const std::vector<Component*>& inComponents);

    private:
        Renderer::Internals                         m_rendererInternals;

        Level*                                      m_level;
        std::vector<MeshComponent*>                 m_meshes;

        std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;
        vk::DescriptorSet                           m_descriptorSet;

        Descriptor::Bundle                          m_frameDescriptor;
        Descriptor::Bundle                          m_textureDescriptor;

        Buffer::Instance                            m_modelVertexBuffer;
        Buffer::Instance                            m_modelIndexBuffer;

        Texture::Manager*                           m_textureManager;
        Model::Manager*                             m_modelManager;

        std::vector<vk::ClearValue>                 m_clearValues;
    };
}