#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            namespace TrueType
            {
                CHICANE_BOX ParsedData parse(const std::vector<unsigned char>& inData);
            }
        }
    }
}