#pragma once

#include "Chicane/Base.hpp"
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
            struct Props : Component::BaseProps
            {
                // onClick
                Component::Event           onClickEvent = {}; // Optional
                Component::Event::Function onClick; // Optional
            };

            void assertProps(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}