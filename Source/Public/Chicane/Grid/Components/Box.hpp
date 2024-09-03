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
            const std::string TAG_ID = "Box";

            // Types
            struct Props
            {
                std::string id = ""; // Optional
                Style style    = {}; // Optional
            };
    
            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}