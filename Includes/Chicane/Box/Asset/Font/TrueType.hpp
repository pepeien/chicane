#pragma once

#include "Chicane/Box/Asset/Font/Parsed.hpp"
#include "Chicane/Box/Asset/Font/Types.hpp"
#include "Chicane/Box/Essential.hpp"

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