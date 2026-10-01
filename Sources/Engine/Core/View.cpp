#include "Chicane/Core/View.hpp"
#include "Chicane/Core/View/Settings.reflected.hpp"

namespace Chicane
{
    void View::flipY()
    {
        projection[1][1] *= -1.0f;
    }

    void View::depthZeroToOne()
    {
        Mat4 depth  = Mat4::One;
        depth[2][2] = 0.5f;
        depth[3][2] = 0.5f;

        projection = depth * projection;
    }
}
