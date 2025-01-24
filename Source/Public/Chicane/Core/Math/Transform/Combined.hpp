#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Transform/Instance.hpp"

namespace Chicane
{
    namespace Transform
    {
        struct Combined : public Instance
        {
        public:
            void combineTransforms();

        public:
            Instance relative = {};
            Instance world    = {};
        };
    }
}