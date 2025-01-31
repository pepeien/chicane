#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Popup
        {
            const std::string TAG_ID = "Popup";

            // Attributes
            const std::string IS_OPEN_ATTRIBUTE_NAME        = "isOpen";
            const std::string IS_CONSTRAINED_ATTRIBUTE_NAME = "isConstrained";
            const std::string IS_RESIZABLE_ATTRIBUTE_NAME   = "isResizable";

            // Props
            struct Props : Component::BaseProps
            {
                bool bIsConstrained = true; // Optional
                bool bIsResizable   = true; // Optional
            };

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}