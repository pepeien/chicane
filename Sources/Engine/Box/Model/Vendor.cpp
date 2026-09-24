#include "Chicane/Box/Model/Vendor.hpp"

namespace Chicane
{
    String toString(Box::ModelVendor inValue)
    {
        switch (inValue)
        {
        case Box::ModelVendor::Undefined:
            return "Undefined";

        case Box::ModelVendor::Wavefront:
            return "Wavefront";

        case Box::ModelVendor::Gltf:
            return "Gltf";

        case Box::ModelVendor::Cooked:
            return "Cooked";

        default:
            return "";
        }
    }
}
