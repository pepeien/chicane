#pragma once

#include <cstdint>

namespace Chicane
{
    struct TracerLocation
    {
    public:
        const char*   name;
        const char*   function;
        const char*   file;
        std::uint32_t line;
        std::uint32_t color;
    };
}
