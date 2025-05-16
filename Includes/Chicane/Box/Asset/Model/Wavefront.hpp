#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Model/Data.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Wavefront
            {
                CHICANE_BOX ParsedData parse(const std::vector<unsigned char>& inData);
            }
        }
    }
}