#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Grid/View.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    class UILayer : public Layer
    {
    public:
        UILayer();
        ~UILayer();

    public:
        void build() override;
        void rebuild() override;

        void onEvent(const SDL_Event& inEvent) override;

        void setup(Vulkan::Frame::Instance& outFrame) override;
        void render(
            Vulkan::Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) override;

    private:
        void initDescriptorPool();
        void initRenderpass();
        void initFramebuffers();
        void initImgui();

    private:
        Window*                     m_window;
        Vulkan::Renderer::Internals m_internals;

        vk::RenderPass              m_renderPass;
        vk::DescriptorPool          m_descriptorPool;

        std::vector<vk::ClearValue> m_clearValues;
    };
}