#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID Popup : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "Popup";

            // Attributes
            static constexpr inline const char* IS_OPEN_ATTRIBUTE_NAME        = "isOpen";
            static constexpr inline const char* IS_CONSTRAINED_ATTRIBUTE_NAME = "isConstrained";
            static constexpr inline const char* IS_RESIZABLE_ATTRIBUTE_NAME   = "isResizable";

        public:
            Popup(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}