#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Buffer.hpp"

namespace Chicane
{
    namespace Mesh
    {
        struct Data
        {
            Mat<4, float> transform;
            Vec<4, float> textureIndex;
        };

        struct Bundle
        {
            size_t            allocationSize;
            void*             writeLocation;
            Buffer::Instance  buffer;
            std::vector<Data> data;
        };
    }
}