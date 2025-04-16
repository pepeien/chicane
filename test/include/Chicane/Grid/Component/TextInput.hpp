#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/Variable.hpp"
#include "Chicane/Grid/Component/Function.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextInput
        {
            const std::string TAG_ID = "TextInput";

            // Attributes
            const std::string LABEL_ATTRIBUTE_NAME    = "label";
            const std::string VALUE_ATTRIBUTE_NAME    = "value";
            const std::string ON_INPUT_ATTRIBUTE_NAME = "onInput";
            const std::string ON_INPUT_EVENT_KEYWORD  = "$event";

            // Props
            struct CHICANE Props : Component::BaseProps
            {
                // Attributes
                Reference           value = {};      // Required
                std::string         label = ""; // Optional

                // Events
                Component::Event    onInputEvent = {}; // Optional
                Component::Function onInput;           // Optional
            };

            CHICANE void assertProps(const pugi::xml_node& inNode);
            CHICANE void compile(const pugi::xml_node& inNode);
        }
    }
}