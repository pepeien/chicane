#pragma once

#include "Chicane/Base.hpp"

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
                    void parse(Parse::Result& outResult, const std::vector<unsigned char>& inData);
                }
            }
        }
    }
}