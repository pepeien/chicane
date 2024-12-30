#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Model
    {
        struct AllocationInfo
        {
        public:
            std::uint32_t vertexCount   = 0;
            std::uint32_t firstVertex   = 0;
            std::uint32_t firstInstance = 0;
            std::uint32_t indexCount    = 0;
            std::uint32_t firstIndex    = 0;
        };
    }
}