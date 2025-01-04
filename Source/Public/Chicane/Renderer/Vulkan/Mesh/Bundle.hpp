#pragma once

#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Mesh/Data.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Mesh
        {
            struct Bundle
            {
            public:
                size_t                           allocationSize;
                void*                            writeLocation;
                Buffer::Instance                 buffer;
                std::vector<Chicane::Mesh::Data> data;
            };
        }
    }
}