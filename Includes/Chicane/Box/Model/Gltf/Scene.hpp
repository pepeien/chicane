#pragma once

#include <vector>

#include "Chicane/Box/Model/Gltf/Instance.hpp"
#include "Chicane/Box/Model/Parsed.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace ModelGltf
        {
            struct CHICANE_BOX Scene
            {
            public:
                ModelParsed::Map      geometry  = {};
                std::vector<Instance> instances = {};
            };
        }
    }
}
