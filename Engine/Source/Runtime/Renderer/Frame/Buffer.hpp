#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Engine
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
                std::vector<Instance>& frames;
            };

            void init(CreateInfo& outCreateInfo);
        }
    }
}