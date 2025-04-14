#pragma once

#include "Chicane/Core/Image.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            struct CreateInfo
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