#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Vulkan/Frame.hpp"

namespace Chicane
{
    class Layer
    {
    public:
        enum class PushStrategy
        {
            Front,
            Back,
            BeforeLayer, // Require to have the ref layer `ID`
            AfterLayer // Require to have the ref layer `ID`
        };

    public:
        Layer()
            : Layer("Unamed")
        {}
        Layer(const std::string& inId)
            : m_id(inId),
            m_bIsInitialized(false)
        {}
        virtual ~Layer() = default;

    public:
        // Lock
        virtual bool canRender() const { return false; }

        // Initialization
        virtual void build() { return; }
        virtual void destroy() { return; }
        virtual void rebuild() { return; }

        // Event
        virtual void onEvent(const SDL_Event& inEvent) { return; }

        // Rendering
        virtual void setup(Vulkan::Frame::Instance& outFrame) { return; }
        virtual void render(
            Vulkan::Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        ) { return; }

    public:
        const std::string& getId() const { return m_id; }

    protected:
        bool m_bIsInitialized;

        std::string m_id;
    };
}