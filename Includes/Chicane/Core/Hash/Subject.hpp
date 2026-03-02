#pragma once

#include <cstddef>

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Hash
    {
        struct CHICANE_CORE Subject
        {
        public:
            const void* data = nullptr;
            std::size_t size = 0;
        };
    }
}