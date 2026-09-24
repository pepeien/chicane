#include "Chicane/Core/Image/Vendor.hpp"

namespace Chicane
{
    String toString(ImageVendor inValue)
    {
        switch (inValue)
        {
        case ImageVendor::Undefined:
            return "Undefined";

        case ImageVendor::Jpg:
            return "Jpg";

        case ImageVendor::Png:
            return "Png";

        case ImageVendor::Gif:
            return "Gif";

        case ImageVendor::Hdr:
            return "Hdr";

        default:
            return "";
        }
    }
}
