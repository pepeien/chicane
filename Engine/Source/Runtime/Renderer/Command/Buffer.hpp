#pragma once

#include "Base.hpp"

#include "Renderer/Frame/Instance.hpp"

namespace Engine
{
    namespace Command
    {
        namespace Buffer
        {
            struct CreateInfo
            {
                vk::Device logicalDevice;
                vk::CommandPool commandPool;
                std::vector<Frame::Instance>& frames;
            };

            void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo);
        }
    }
}