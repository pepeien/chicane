#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Asset/Font/Raw.hpp"
#include "Chicane/Box/Asset/Font/Vendor.hpp"

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