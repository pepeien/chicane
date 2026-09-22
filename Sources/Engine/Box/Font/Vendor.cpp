#include "Chicane/Box/Font/Vendor.hpp"

namespace Chicane
{
    String toString(Box::FontVendor inValue)
    {
        switch (inValue)
        {
        case Box::FontVendor::Undefined:
            return "Undefined";

        case Box::FontVendor::OpenType:
            return "OpenType";

        case Box::FontVendor::TrueType:
            return "TrueType";

        default:
            return "";
        }
    }
}
