#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ButtonComponent
        {
            const std::string TAG_ID = "Button";
    
            // Attributes
            const std::string ON_CLICK_ATTRIBUTE_NAME = "onClick";
    
            void compile(pugi::xml_node& outNode);
        }
    }
}