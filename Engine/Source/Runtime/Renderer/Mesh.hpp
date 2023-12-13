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
                
                struct ManagerCreateInfo
                {
                    vk::Device logicalDevice;
                    vk::PhysicalDevice physicalDevice;
                    vk::Queue queue;
                    vk::CommandBuffer commandBuffer;
                };

                class Manager
                {
                public:
                    Manager(ManagerCreateInfo& inCreateInfo);
                    ~Manager();

                public:
                    void addMesh(std::vector<Vertex::Base*> inVertices);
                    void proccess();

                    std::vector<AllocationInfo> getAllocationInfoList();

                private:
                    void extractVerticesFromMeshList(
                        std::vector<Vertex::Base>& outVertices,
                        vk::DeviceSize& outAllocationSize,
                        std::vector<AllocationInfo>& outAllocationInfoList,
                        std::vector<std::vector<Vertex::Base*>>& inMeshes
                    );

                    void destroyBuffer(Vertex::Buffer& inBuffer);

                public:
                    Vertex::Buffer vertexBuffer;

                private:
                    vk::Device logicalDevice;
                    vk::PhysicalDevice physicalDevice;
                    vk::Queue queue;
                    vk::CommandBuffer commandBuffer;

                    std::vector<std::vector<Vertex::Base*>> meshes;

                    std::vector<AllocationInfo> allocationInfoList;
                };
            }
        }
    }
}