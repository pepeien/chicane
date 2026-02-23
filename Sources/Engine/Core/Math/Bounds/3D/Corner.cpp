#include "Chicane/Core/Math/Bounds/3D/Corner.hpp"

namespace Chicane
{
    Bounds3DCorner::Bounds3DCorner(const Vec3& inBase, const Vec3& inTransformed)
        : base(inBase),
          transformed(inTransformed)
    {}

    Bounds3DCorner::Bounds3DCorner()
        : base(Vec3::Zero),
          transformed(Vec3::Zero)
    {}
}