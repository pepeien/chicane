#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID TextInput : public Component
        {
        public:
            // Tag
            static inline constexpr const char* TAG_ID = "TextInput";

            // Attributes
            static inline constexpr const char* LABEL_ATTRIBUTE_NAME    = "label";
            static inline constexpr const char* VALUE_ATTRIBUTE_NAME    = "value";
            static inline constexpr const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";
            static inline constexpr const char* ON_INPUT_EVENT_KEYWORD  = "$event";

        public:
            TextInput(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}