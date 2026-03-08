#include "Chicane/Core/Math/Vec/Vec4.reflected.hpp"

namespace Chicane
{
    String Vec4::toString() const
    {
        return String::sprint("[%.2f, %.2f, %.2f, %2f]", x, y, z, w);
    }
}