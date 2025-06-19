#pragma once

#include "Chicane/Grid/Component.hpp"
#include "Chicane/Grid/Essential.hpp"

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

        protected:
            void onRefresh() override;

            void refreshPrimitive() override;

        protected:
            void refreshColor();

        protected:
            bool        m_bCanUpdate;

            Color::Rgba m_color;
        };
    }
}