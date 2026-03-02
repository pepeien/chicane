#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Font/Family.hpp"
#include "Chicane/Box/Font/Raw.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace FontTrueType
        {
            CHICANE_BOX FontFamily parse(const String& inFamily, const FontRaw& inData);
        }
    }
}