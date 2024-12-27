#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ButtonComponent
        {
            const std::string TAG_ID = "Button";

            // Attributes
            const std::string ON_CLICK_ATTRIBUTE_NAME = "onClick";
            const std::string ON_CLICK_EVENT_KEYWORD  = "$event";

            // Props
            struct Props : Component::BaseProps
            {
                // onClick
                Component::Event    onClickEvent = {}; // Optional
                Component::Function onClick; // Optional
            };

            void validate(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}