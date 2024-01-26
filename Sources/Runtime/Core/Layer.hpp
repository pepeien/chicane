#pragma once

#include "Base.hpp"

#include "Renderer/Frame.hpp"

namespace Chicane
{
    class Layer
    {
    public:
        Layer();
        Layer(const std::string& inName);
        virtual ~Layer() = default;

    public:
        virtual void setup();
        virtual void render(
            Frame::Instance& outFrame,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Extent2D& inSwapChainExtent
        );

    protected:
        std::string m_name;
    };
}