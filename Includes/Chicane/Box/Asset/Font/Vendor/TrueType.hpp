#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            namespace Vendor
            {
                namespace TrueType
                {
                    CHICANE_BOX RawData parse(const std::vector<unsigned char>& inData);
                }
            }
        }
    }
}