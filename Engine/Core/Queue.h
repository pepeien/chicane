#pragma once

#include <optional>

namespace Engine
{
    namespace Core
    {
        struct QueueFamilyIndices
        {
        public:
            bool isComplete();

        public:
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
        };
    }
}