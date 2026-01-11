#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Font/Parsed.hpp"
#include "Chicane/Box/Asset/Font/Raw.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace FontTrueType
        {
            CHICANE_BOX FontParsed parse(const FontRaw& inData);
        }
    }
}