#include "Chicane/Core/Math/Vec/Vec2.reflected.hpp"

namespace Chicane
{
    String Vec2::toString() const
    {
        return String::sprint("[%.2f, %.2f]", x, y);
    }
}