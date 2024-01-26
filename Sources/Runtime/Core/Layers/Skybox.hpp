#pragma once

#include "Base.hpp"

#include "Core/Layer.hpp"
#include "Game/Level.hpp"

#include "Renderer.hpp"

namespace Chicane
{
    class SkyboxLayer : public Layer
    {
    public:
        SkyboxLayer(Renderer* inRenderer);
        ~SkyboxLayer();

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

        // Texture
        std::unique_ptr<CubeMap::Manager> m_manager;

        // Descriptors
        Descriptor::Bundle m_frameDescriptor;
        Descriptor::Bundle m_materialDescriptor;
    };
}