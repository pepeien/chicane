#pragma once

#include "Runtime/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextComponent
        {
            const std::string TAG_ID = "Text";
    
            void compile(pugi::xml_node& outNode);
        }
    }
}