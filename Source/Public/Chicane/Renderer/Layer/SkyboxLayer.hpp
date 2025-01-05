#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    class SkyboxLayer : public Layer
    {
    public:
        SkyboxLayer();
        ~SkyboxLayer();

    public:
        void build() override;
        void destroy() override;
        void rebuild() override;

        void render(
            Vulkan::Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void initFrameDescriptorSetLayout();
        void initMaterialDescriptorSetLayout();
        void initGraphicsPipeline();
        void initFramebuffers();
        void initFrameResources();
        void initMaterialResources();
        void buildAssets();

    private:
        Vulkan::Renderer::Internals                         m_internals;

        std::unique_ptr<Vulkan::GraphicsPipeline::Instance> m_graphicsPipeline;

        vk::DescriptorSet                                   m_descriptorSet;
        Vulkan::Descriptor::Bundle                          m_frameDescriptor;
        Vulkan::Descriptor::Bundle                          m_materialDescriptor;

        std::vector<vk::ClearValue>                         m_clearValues;
    };
}