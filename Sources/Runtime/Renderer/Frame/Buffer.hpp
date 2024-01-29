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
                std::string id;
                vk::Device logicalDevice;
                vk::RenderPass renderPass;
                vk::Extent2D swapChainExtent;
                std::vector<vk::ImageView> attachments;
            };

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