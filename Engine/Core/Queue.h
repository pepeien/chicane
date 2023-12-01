#pragma once

#include <optional>

namespace Engine
{
    namespace Core
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
        }
    }
}