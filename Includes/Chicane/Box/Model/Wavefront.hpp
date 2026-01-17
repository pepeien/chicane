#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Model/Extracted.hpp"
#include "Chicane/Box/Model/Raw.hpp"

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