#pragma once

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Queue
        {
            struct CHICANE_RUNTIME FamilyIndices
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