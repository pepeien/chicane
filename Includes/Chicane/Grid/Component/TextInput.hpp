#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE TextInput : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "TextInput";

            // Attributes
            static constexpr const char* LABEL_ATTRIBUTE_NAME    = "label";
            static constexpr const char* VALUE_ATTRIBUTE_NAME    = "value";
            static constexpr const char* ON_INPUT_ATTRIBUTE_NAME = "onInput";
            static constexpr const char* ON_INPUT_EVENT_KEYWORD  = "$event";

        public:
            TextInput(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}