#pragma once

#include "Chicane/Core/Color.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Container : public Component
        {
        public:
            // Tag
            static inline constexpr const char* TAG_ID = "Container";

        public:
            Container(const pugi::xml_node& inNode);
        };
    }
}