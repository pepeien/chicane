#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"
#include "Chicane/Box/Asset/Font/Parsed.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            namespace TrueType
            {
                CHICANE_BOX Parsed parse(const Raw& inData);
            }
        }
    }
}