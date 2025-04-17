#pragma once

#include "Core/Image.hpp"
#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            struct CHICANE CreateInfo
            {
            public:
                Chicane::Image::CompiledData image;

                vk::Device                   logicalDevice;
                vk::PhysicalDevice           physicalDevice;
                vk::CommandBuffer            commandBuffer;
                vk::Queue                    queue;
            };
        }
    }
}