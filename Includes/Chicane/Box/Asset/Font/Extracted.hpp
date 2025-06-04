#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Data.hpp"
#include "Chicane/Box/Asset/Font/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            struct CHICANE_BOX Extracted
            {
            public:
                Vendor vendor = Vendor::Undefined;
                Raw    data   = {};
            };
        }
    }
}