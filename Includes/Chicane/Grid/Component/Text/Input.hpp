#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID TextInput : public Component
        {
        public:
            // Tag
            static constexpr inline const char* TAG_ID = "TextInput";

            // Attributes
            static constexpr inline const char* LABEL_ATTRIBUTE_NAME    = "label";
            static constexpr inline const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr inline const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";

        public:
            TextInput(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}