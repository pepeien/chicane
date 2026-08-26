#include "Chicane/Grid/Style/Transform.hpp"

#include <cfloat>
#include <cmath>

namespace Chicane
{
    namespace Grid
    {
        StyleTransform::StyleTransform()
            : translation(Vec2::Zero()),
              rotation(0.0f),
              scale(Vec2::One())
        {}

        bool StyleTransform::isIdentity() const
        {
            return std::fabs(translation.x) < FLT_EPSILON && std::fabs(translation.y) < FLT_EPSILON &&
                   std::fabs(rotation) < FLT_EPSILON && std::fabs(scale.x - 1.0f) < FLT_EPSILON &&
                   std::fabs(scale.y - 1.0f) < FLT_EPSILON;
        }
    }
}
