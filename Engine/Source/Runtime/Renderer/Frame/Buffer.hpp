#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/Command/Buffer.hpp"

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
                const Command::Buffer::CreateInfo& inCreateInfo
            );
            void initCommand(
                Instance& outFrame,
                const Command::Buffer::CreateInfo& inCreateInfo
            );
        }
    }
}