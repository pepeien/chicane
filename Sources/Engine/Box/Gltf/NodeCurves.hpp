#pragma once

#include "SamplerCurve.hpp"

namespace Chicane
{
    namespace Box
    {
        struct NodeCurves
        {
            SamplerCurve translation;
            SamplerCurve rotation;
            SamplerCurve scale;
        };

    }
}
