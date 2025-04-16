#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct Style;

        namespace Text
        {
            const std::string TAG_ID = "Text";

            CHICANE void compileRaw(const std::string& inText, const Style& inStyle);
            CHICANE void compile(const pugi::xml_node& inNode);
        }
    }
}