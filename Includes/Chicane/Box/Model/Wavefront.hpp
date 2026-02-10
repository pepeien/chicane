#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Model/Parsed.hpp"
#include "Chicane/Box/Model/Raw.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelWavefront
        {
            CHICANE_BOX ModelParsed::Map parse(const ModelRaw& inData);
        }
    }
}