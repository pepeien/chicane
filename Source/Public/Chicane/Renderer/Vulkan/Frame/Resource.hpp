#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            template<typename T>
            struct Resource
            {
            public:
                size_t                   allocationSize;
                void*                    writeLocation;
                Vulkan::Buffer::Instance buffer;
                T                        data;
            };
        }
    }
}