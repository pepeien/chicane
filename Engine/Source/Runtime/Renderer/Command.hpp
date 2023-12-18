#pragma once

#include "Base.hpp"

#include "Frame.hpp"

namespace Engine
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
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::SurfaceKHR& inSurface
        );
    
        void initBuffers(vk::CommandBuffer& outCommandBuffer, const BufferCreateInfo& inCreateInfo);

        void startJob(const vk::CommandBuffer& inCommandBuffer);
        void endJob(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const std::string& inDescription = "Commad Buffer"
        );
    }
}