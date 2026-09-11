#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX SkeletonBoneEntry
        {
        public:
            using List = std::vector<SkeletonBoneEntry>;

        public:
            SkeletonBoneEntry();

        public:
            String       id;
            std::int32_t parentIndex;
            Transform    transform;
            Mat4         inverseBind;
        };
    }
}
