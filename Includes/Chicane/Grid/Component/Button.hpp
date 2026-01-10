#pragma once

#include "Chicane/Core/Xml.hpp"
#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Button : public Component
        {
        public:
            // Header
            static inline constexpr const char* TAG_ID = "Button";

            // Attributes
            static inline constexpr const char* ON_CLICK_ATTRIBUTE_NAME =
                "onClick";
            static inline constexpr const char* ON_CLICK_EVENT_KEYWORD =
                "$event";

        public:
            Button(const pugi::xml_node& inNode);

        protected:
            virtual void onClick(const Event& inEvent) { return; }
        };
    }
}