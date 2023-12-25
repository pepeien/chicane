#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/CommandBuffer/Instance.hpp"

namespace Chicane
{
    namespace Frame
    {
        namespace Buffer
        {
            struct CreateInfo
            {
                vk::Device logicalDevice;
                vk::RenderPass renderPass;
                vk::Extent2D swapChainExtent;
            };

            void init(std::vector<Instance>& outFrames, const CreateInfo& inCreateInfo);
            void init(Instance& outFrame, const CreateInfo& inCreateInfo);

            void initCommand(
                std::vector<Instance>& outFrames,
                const CommandBuffer::Instance::CreateInfo& inCreateInfo
            );
            void initCommand(
                Instance& outFrame,
                const CommandBuffer::Instance::CreateInfo& inCreateInfo
            );
        }
    }
}