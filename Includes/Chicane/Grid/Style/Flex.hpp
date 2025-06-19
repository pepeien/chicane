#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFlex
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