#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Model/Parse/Result.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            namespace Vendor
            {
                namespace Wavefront
                {
                    CHICANE_BOX void parse(Parse::Result& outResult, const std::vector<unsigned char>& inData);
                }
            }
        }
    }
}