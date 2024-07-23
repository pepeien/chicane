#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/CommandBuffer.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/GraphicsPipeline.hpp"

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