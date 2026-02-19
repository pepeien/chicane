#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Font/Parsed.hpp"
#include "Chicane/Box/Font.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace FontTrueType
        {
            CHICANE_BOX FontParsed parse(const Font& inAsset);
        }
    }
}