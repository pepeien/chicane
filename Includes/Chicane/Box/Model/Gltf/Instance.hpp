#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelGltf
        {
            struct CHICANE_BOX Instance
            {
            public:
                String    id;
                String    bone;
                Transform transform;
            };
        }
    }
}
