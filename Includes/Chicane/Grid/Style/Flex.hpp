#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Style/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE_GRID Flex
            {
            public:
                enum class Direction : std::uint8_t
                {
                    Column,
                    Row
                };

            public:
                Direction direction = Direction::Row;
            };
        }
    }
}