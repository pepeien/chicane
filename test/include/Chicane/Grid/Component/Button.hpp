#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/Function.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Button
        {
            const std::string TAG_ID = "Button";

            // Attributes
            const std::string ON_CLICK_ATTRIBUTE_NAME = "onClick";
            const std::string ON_CLICK_EVENT_KEYWORD  = "$event";

            // Props
            struct CHICANE Props : Component::BaseProps
            {
                // onClick
                Component::Event           onClickEvent = {}; // Optional
                Component::Event::Function onClick; // Optional
            };

            CHICANE void assertProps(const Props& inProps);

            CHICANE void compileRaw(const Props& inProps);
            CHICANE void compile(const pugi::xml_node& inNode);
        }
    }
}