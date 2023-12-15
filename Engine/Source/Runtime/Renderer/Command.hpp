#pragma once

#include "Base.hpp"

#include "Frame.hpp"
#include "Queue.hpp"
#include "SwapChain.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Command
            {
                struct BufferCreateInfo
                {
                    vk::Device logicalDevice;
                    vk::CommandPool commandPool;
                    std::vector<Frame::Instance>& frames;
                };

                void initPool(
                    vk::CommandPool& outCommandPool,
                    vk::Device& inLogicalDevice,
                    vk::PhysicalDevice& inPhysicalDevice,
                    vk::SurfaceKHR& inSurface
                );

                void initBuffers(vk::CommandBuffer& outCommandBuffer, BufferCreateInfo& inCreateInfo);
            }
        }
    }
}