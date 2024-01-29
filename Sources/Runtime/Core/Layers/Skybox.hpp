#pragma once

#include "Base.hpp"

#include "Core/Layer.hpp"
#include "Game/Level.hpp"

#include "Renderer.hpp"

namespace Chicane
{
    class Window;

    class SkyboxLayer : public Layer
    {
    public:
        SkyboxLayer(Window* inWindow);

    public:
        void init() override;
        void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;
        void destroy() override;

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
        Level* m_level;

        std::unique_ptr<GraphicsPipeline::Instance> m_graphicsPipeline;

        vk::DescriptorSet m_descriptorSet;
        Descriptor::Bundle m_frameDescriptor;
        Descriptor::Bundle m_materialDescriptor;

        std::unique_ptr<CubeMap::Manager> m_manager;
    };
}