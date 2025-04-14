#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace View
    {
        struct Data
        {
        public:
            // Settings
            Vec<4, float> clip                   = Vec<4, float>(0.1f, 1000.0f, 0.0f, 0.0f);

            // Matrices
            Mat<4, float> projection             = Mat<4, float>(1.0f);
            Mat<4, float> view                   = Mat<4, float>(1.0f);
            Mat<4, float> viewProjection         = Mat<4, float>(1.0f);
            Mat<4, float> inversedViewProjection = Mat<4, float>(1.0f);

            // Orientantion
            Vec<4, float> right                  = Vec<4, float>(RIGHT_DIRECTION,   0.0f);
            Vec<4, float> forward                = Vec<4, float>(FORWARD_DIRECTION, 0.0f);
            Vec<4, float> up                     = Vec<4, float>(UP_DIRECTION,      0.0f);

            // Transform
            Vec<4, float> translation            = Vec<4, float>(0.0f);
        };
    }
}