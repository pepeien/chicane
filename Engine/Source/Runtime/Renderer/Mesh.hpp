#pragma once

#include <map>
#include <algorithm>

#include "Base.hpp"

#include "Vertex/2D.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
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

                class Manager
                {
                public:
                    Manager(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice);
                    ~Manager();

                public:
                    void addMesh(std::vector<Vertex::Base*> inVertices);
                    void proccess();

                    std::vector<AllocationInfo> getAllocationInfoList();

                private:
                    void extractVerticesFromMeshList(
                        std::vector<Vertex::Base>& outVertices,
                        size_t& outAllocationSize,
                        std::vector<AllocationInfo>& outAllocationInfoList,
                        std::vector<std::vector<Vertex::Base*>>& inMeshes
                    );

                public:
                    Vertex::Buffer vertexBuffer;

                private:
                    vk::Device logicalDevice;
                    vk::PhysicalDevice physicalDevice;

                    std::vector<std::vector<Vertex::Base*>> meshes;

                    std::vector<AllocationInfo> allocationInfoList;
                };
            }
        }
    }
}