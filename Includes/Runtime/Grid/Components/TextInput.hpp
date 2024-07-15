#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextInputComponent
        {
            const std::string TAG_ID = "TextInput";

            // Attributes
            const std::string LABEL_ATTRIBUTE_NAME = "label";
            const std::string VALUE_ATTRIBUTE_NAME = "value";

            void validate(const pugi::xml_node& inNode);
            void compile(pugi::xml_node& outNode);
        }
    }
}