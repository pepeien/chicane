#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Font/Raw.hpp"
#include "Chicane/Box/Font/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX FontExtracted
        {
        public:
            FontVendor vendor = FontVendor::Undefined;
            FontRaw    data   = {};
        };
    }
}