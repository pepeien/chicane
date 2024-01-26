#pragma once

#include "Base.hpp"

#include "Core/Layer.hpp"
#include "Game/Level.hpp"

#include "Renderer.hpp"

namespace Chicane
{
    class LevelLayer : public Layer
    {
    public:
        LevelLayer(Renderer* inRenderer);
        ~LevelLayer();

    public:
        void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void loadAssets();
        void initDescriptors();
        void initGraphicsPipeline();
        void initFramebuffers();
        void initFrameResources();
        void initMaterialResources();
        void buildAssets();

    private:
        Renderer* m_renderer;

        std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;
        vk::DescriptorSet m_descriptorSet;

        // Managers
        std::unique_ptr<Model::Manager::Instance> m_modelManager;
        std::unique_ptr<Texture::Manager::Instance> m_textureManager;

        // Descriptors
        Descriptor::Bundle m_frameDescriptor;
        Descriptor::Bundle m_materialDescriptor;

        // Assets
        Buffer::Instance m_meshVertexBuffer;
        Buffer::Instance m_meshIndexBuffer;
    };
}