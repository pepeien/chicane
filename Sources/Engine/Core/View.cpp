#include "Chicane/Core/View.hpp"

namespace Chicane
{
    static const Mat4 ZERO_TO_ONE =
        Mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

    void View::flipY()
    {
        projection[1][1] *= -1.0f;
    }

    void View::depthToZeroOne()
    {
        return;

        projection = ZERO_TO_ONE * projection;
    }
}