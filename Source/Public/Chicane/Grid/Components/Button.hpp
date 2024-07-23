#pragma once

#include "Chicane/Grid/Essential.hpp"

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
    
            void compile(const pugi::xml_node& outNode);
        }
    }
}