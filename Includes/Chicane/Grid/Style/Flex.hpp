#pragma once

#include "Core.hpp"
#include "Grid/Style/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            struct CHICANE Flex
            {
            public:
                enum class Direction : std::uint8_t
                {
                    Column,
                    Row
                };

            public:
                std::string direction = FLEX_DIRECTION_TYPE_ROW;
            };
        }
    }
}