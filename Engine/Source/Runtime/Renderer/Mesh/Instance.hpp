#pragma once

#include "Base.hpp"

#include "Renderer/Vertex/Buffer.hpp"
#include "Renderer/Vertex/Instance.hpp"

namespace Engine
{
    namespace Mesh
    {
        struct AllocationInfo
        {
            uint32_t vertexCount;
            uint32_t instanceCount;
            uint32_t firstVertex;
            uint32_t firstInstance;
        };

        struct Instance
        {
            std::vector<Vertex::Base*> vertices;
            AllocationInfo allocationInfo;
        };
    }
}