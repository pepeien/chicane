#pragma once

#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Button : public Container
        {
        public:
            // Header
            static inline constexpr const char* TAG_ID = "Button";

        public:
            Button(const pugi::xml_node& inNode);
        };
    }
}