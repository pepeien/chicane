#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Core
    {
        namespace Render
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

                void findFamilyInidices(FamilyIndices& allocator, vk::PhysicalDevice& inDevice, vk::SurfaceKHR& inSurface);
            }
        }
    }
}