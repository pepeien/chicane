#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Mesh/Data.hpp"

namespace Chicane
{
    namespace Mesh
    {
        struct Bundle
        {
        public:
            size_t            allocationSize;
            void*             writeLocation;
            Buffer::Instance  buffer;
            std::vector<Data> data;
        };
    }
}