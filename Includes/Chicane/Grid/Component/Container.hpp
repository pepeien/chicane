#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

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
            bool m_canUpdate;

            Color::Rgba m_color;
        };
    }
}