#include "Chicane/Core/View/Projection/Type.hpp"
#include "Chicane/Core/View/Projection/Type.reflected.hpp"

namespace Chicane
{
    String toString(ViewProjectionType inValue)
    {
        switch (inValue)
        {
        case ViewProjectionType::Orthographic:
            return "Orthographic";

        case ViewProjectionType::Perspective:
            return "Perspective";

        default:
            return "";
        }
    }
}
