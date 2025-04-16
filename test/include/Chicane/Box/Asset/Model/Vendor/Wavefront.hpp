#pragma once

#include "Chicane/Core.hpp"

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
                    CHICANE void parse(Parse::Result& outResult, const std::vector<unsigned char>& inData);
                }
            }
        }
    }
}