#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace TextComponent
        {
            const std::string TAG_ID = "Text";

            void compile(const pugi::xml_node& inNode);
            void compileRaw(const std::string& inText);
        }
    }
}