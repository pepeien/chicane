#pragma once

#include "Runtime/Runtime.hpp"

#include "Runtime/Renderer/Frame.hpp"

namespace Chicane
{
    class Layer
    {
    public:
        Layer()
            : Layer("Unamed")
        {}
        Layer(const std::string& inId)
            : m_id(inId),
            m_isInitialized(false)
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

    public:
        std::string getId() { return m_id; }

    protected:
        bool m_isInitialized;

        std::string m_id;
    };
}