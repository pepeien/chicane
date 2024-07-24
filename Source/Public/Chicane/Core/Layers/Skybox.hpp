#pragma once

#include "Chicane/Base.hpp"

#include "Chicane/Core/Layer.hpp"
#include "Chicane/Game/Level.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    class Window;

    class SkyboxLayer : public Layer
    {
    public:
        SkyboxLayer(Window* inWindow);
        ~SkyboxLayer();

    public:
        void build() override;
        void destroy() override;
        void rebuild() override;

        void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void loadAssets();
        void initFrameDescriptorSetLayout();
        void initMaterialDescriptorSetLayout();
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
    };
}