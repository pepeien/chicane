#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ListComponent
        {
            const std::string TAG_ID = "List";

            // Attributes
            const std::string DIRECTION_ATTRIBUTE_NAME = "direction";

            // Direction
            const std::string DIRECTION_ROW    = "ROW";
            const std::string DIRECTION_COLUMN = "COLUMN";

            enum class Direction : uint8_t
            {
                Column,
                Row
            };

            Direction getDirection(const pugi::xml_node& inNode);

            void validate(const pugi::xml_node& inNode);
            void compile(pugi::xml_node& outNode);
        }
    }
}