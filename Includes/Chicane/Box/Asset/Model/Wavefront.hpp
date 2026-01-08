#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Model/Extracted.hpp"
#include "Chicane/Box/Asset/Model/Raw.hpp"

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