#include "Chicane/Core/View.hpp"

namespace Chicane
{
    void View::flipY()
    {
        projection[1][1] *= -1.0f;
    }
}