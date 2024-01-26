#include "Layer.hpp"

#include "Log.hpp"

namespace Chicane
{
    Layer::Layer()
        : Layer("Unamed")
    {}

    Layer::Layer(const std::string& inName)
        : m_name(inName)
    {}

    void Layer::setup()
    {
        LOG_WARNING(m_name + " Layer setup not implemented");
    }

    void Layer::render(
        Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        LOG_WARNING(m_name + " Layer render not implemented");
    }
}