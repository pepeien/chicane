#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Style.hpp"

#include <imgui/imgui_internal.h>

namespace Chicane
{
    namespace Grid
    {
        namespace BoxComponent
        {
            // Types
            struct Props
            {
                std::string id = ""; // Optional
                Style style    = {}; // Optional
            };

            const std::string TAG_ID = "Box";
    
            void compile(const pugi::xml_node& inNode);
        }
    }
}