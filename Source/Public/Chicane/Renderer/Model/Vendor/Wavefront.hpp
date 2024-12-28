#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Model.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Wavefront
        {
            void parse(
                Parse::Result& outResult,
                const std::vector<unsigned char>& inData
            );
        }
    }
}