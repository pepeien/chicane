#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextInputComponent
        {
            const std::string TAG_ID = "TextInput";

            // Attributes
            const std::string LABEL_ATTRIBUTE_NAME    = "label";
            const std::string VALUE_ATTRIBUTE_NAME    = "value";
            const std::string ON_INPUT_ATTRIBUTE_NAME = "onInput";
            const std::string ON_INPUT_EVENT_KEYWORD  = "$event";

            // Props
            struct Props : Component::BaseProps
            {
                // Attributes
                Component::Variable value;      // Required
                std::string         label = ""; // Optional

                // Events
                Component::Event    onInputEvent = {}; // Optional
                Component::Function onInput;           // Optional
            };

            void validate(const pugi::xml_node& inNode);
            void compile(const pugi::xml_node& inNode);
        }
    }
}