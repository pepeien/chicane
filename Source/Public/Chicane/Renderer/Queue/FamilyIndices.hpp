#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
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