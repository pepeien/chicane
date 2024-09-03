#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct Style;

        namespace TextComponent
        {
            const std::string TAG_ID = "Text";

            void compileRaw(const std::string& inText, const Style& inStyle);
            void compile(const pugi::xml_node& inNode);
        }
    }
}