#pragma once

#include "Runtime/Runtime.hpp"

#include "Runtime/Core/Layer.hpp"
#include "Runtime/Game/Level.hpp"

#include "Runtime/Renderer.hpp"

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
        void parseUI(const std::string& inFilepath);

    private:
        Window* m_window;
        Renderer* m_renderer;

        vk::RenderPass m_renderPass;
        vk::DescriptorPool m_descriptorPool;
    };
}