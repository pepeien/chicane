#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Popup : public Component
        {
        public:
            // Tag
            static inline constexpr const char* TAG_ID = "Popup";

            // Attributes
            static inline constexpr const char* IS_OPEN_ATTRIBUTE_NAME =
                "isOpen";
            static inline constexpr const char* IS_CONSTRAINED_ATTRIBUTE_NAME =
                "isConstrained";
            static inline constexpr const char* IS_RESIZABLE_ATTRIBUTE_NAME =
                "isResizable";

        public:
            Popup(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}