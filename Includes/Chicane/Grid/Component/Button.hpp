#pragma once

#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Container.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Button : public Container
        {
        public:
            // Header
            static constexpr inline const char* TAG_ID = "Button";

        public:
            CH_CONSTRUCTOR()
            Button(const pugi::xml_node& inNode);
        };
    }
}