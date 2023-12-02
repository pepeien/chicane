#pragma once

#include "Base.h"

namespace Engine
{
    namespace Core
    {
        namespace Vulkan
        {
            namespace Queue
            {
                struct FamilyIndices
                {
                public:
                    bool isComplete();

                public:
                    std::optional<uint32_t> graphicsFamily;
                    std::optional<uint32_t> presentFamily;
                };

                void findFamilyInidices(FamilyIndices& allocator, vk::PhysicalDevice& inDevice);
            }
        }
    }
}