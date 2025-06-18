#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Model/Extracted.hpp"
#include "Chicane/Box/Asset/Model/Types.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelWavefront
        {
            CHICANE_BOX ModelExtracted parse(const ModelRaw& inData);
        }
    }
}