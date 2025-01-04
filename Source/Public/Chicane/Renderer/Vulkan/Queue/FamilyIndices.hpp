#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
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
                std::optional<std::uint32_t> graphicsFamily;
                std::optional<std::uint32_t> presentFamily;
            };
        }
    }
}