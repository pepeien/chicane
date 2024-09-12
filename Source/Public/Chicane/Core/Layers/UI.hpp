#pragma once

#include "Chicane/Base.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Core/Layer.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Grid/View.hpp"

namespace Chicane
{
    class Window;

    class UILayer : public Layer
    {
    public:
        UILayer(Window* inWindow);
        ~UILayer();

    public:
        void build() override;
        void rebuild() override;

        void onEvent(const SDL_Event& inEvent) override;

        void setup(Frame::Instance& outFrame) override;
        void render(
            Frame::Instance& outFrame,
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
        Renderer::Internals         m_internals;

        vk::RenderPass              m_renderPass;
        vk::DescriptorPool          m_descriptorPool;

        std::vector<vk::ClearValue> m_clearValues;
    };
}