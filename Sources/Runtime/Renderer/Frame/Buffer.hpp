#pragma once

#include "Base.hpp"

#include "Renderer/CommandBuffer.hpp"
#include "Renderer/Frame.hpp"
#include "Renderer/GraphicsPipeline.hpp"

namespace Chicane
{
    namespace Frame
    {
        namespace Buffer
        {
            struct CreateInfo
            {
                vk::Device logicalDevice;
                std::unordered_map<
                    Layer,
                    vk::RenderPass
                > renderPasses;
                vk::Extent2D swapChainExtent;
            };

            void init(
                std::vector<Instance>& outFrames,
                const CreateInfo& inCreateInfo
            );
            void init(
                Instance& outFrame,
                const CreateInfo& inCreateInfo
            );

            void initCommand(
                std::vector<Instance>& outFrames,
                const CommandBuffer::CreateInfo& inCreateInfo
            );
            void initCommand(
                Instance& outFrame,
                const CommandBuffer::CreateInfo& inCreateInfo
            );
        }
    }
}