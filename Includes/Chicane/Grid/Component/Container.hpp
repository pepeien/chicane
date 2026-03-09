#pragma once

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        CH_TYPE(Manual)
        class CHICANE_GRID Container : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Container";

        public:
            CH_CONSTRUCTOR()
            Container(const pugi::xml_node& inNode);

        protected:
            void onRefresh() override;
        };
    }
}