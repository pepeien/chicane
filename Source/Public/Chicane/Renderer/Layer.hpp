#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Vulkan/Frame.hpp"

namespace Chicane
{
    class Layer
    {
    public:
        enum class Status : std::uint8_t
        {
            Idle,
            Initialized
        };

        enum class PushStrategy : std::uint8_t
        {
            Front,
            Back,
            BeforeLayer, // Require to have the ref layer `ID`
            AfterLayer   // Require to have the ref layer `ID`
        };

    public:
        Layer()
            : Layer("Unamed")
        {}
        Layer(const std::string& inId)
            : m_id(inId),
              m_status(Status::Idle)
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
        bool isStatus(Status inStatus) const { return m_status == inStatus; };

        const std::string& getId() const { return m_id; }

    protected:
        std::string m_id;
        Status      m_status;
    };
}