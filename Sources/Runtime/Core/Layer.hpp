#pragma once

#include "Base.hpp"

#include "Renderer/Frame.hpp"

namespace Chicane
{
    class Layer
    {
    public:
        Layer()
            : Layer("Unamed")
        {}
        Layer(const std::string& inName)
            : m_name(inName)
        {}
        virtual ~Layer() = default;

    public:
        // Initialization
        virtual void build() { return; }
        virtual void destroy() { return; }
        virtual void rebuild() { return; }

        // Event
        virtual void onEvent(const SDL_Event& inEvent) { return; }

        // Rendering
        virtual void setup(Frame::Instance& outFrame) { return; }
        virtual void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) { return; }

    protected:
        std::string m_name;
    };
}