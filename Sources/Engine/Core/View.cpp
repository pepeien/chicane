#include "Chicane/Core/View.hpp"

namespace Chicane
{
    void View::flipY()
    {
        projection[1][1] *= -1.0f;
    }

    void View::depthZeroToOne()
    {
        // Convert GLM clip Z [-1, 1] to API clip Z [0, 1].
        Mat4 depth  = Mat4::One;
        depth[2][2] = 0.5f;
        depth[3][2] = 0.5f;

        projection = depth * projection;
    }
}
