#pragma once

#include "Runtime/Grid/Essential.hpp"

#include <imgui/imgui_internal.h>

namespace Chicane
{
    namespace Grid
    {
        namespace BoxComponent
        {
            const std::string TAG_ID = "Box";
    
            void compile(pugi::xml_node& outNode);
        }
    }
}