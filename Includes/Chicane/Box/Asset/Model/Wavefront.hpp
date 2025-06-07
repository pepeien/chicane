#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Model/Extracted.hpp"
#include "Chicane/Box/Asset/Model/Types.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Wavefront
            {
                CHICANE_BOX Extracted parse(const Raw& inData);
            }
        }
    }
}