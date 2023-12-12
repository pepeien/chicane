#include "Mesh.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Mesh
            {
                Manager::Manager(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice)
                {
                    logicalDevice  = inLogicalDevice;
                    physicalDevice = inPhysicalDevice;
                }

                Manager::~Manager()
                {
                    logicalDevice.destroyBuffer(vertexBuffer.instance);
                    logicalDevice.freeMemory(vertexBuffer.memory);
                }

                void Manager::addMesh(std::vector<Vertex::Base*> inVertices)
                {
                    meshes.push_back(inVertices);
                }

                void Manager::proccess()
                {
                    size_t allocationSize = 0;

                    std::vector<Vertex::Base> extractedVertices;

                    extractVerticesFromMeshList(
                        extractedVertices,
                        allocationSize,
                        allocationInfoList,
                        meshes
                    );
  
                    Vertex::BufferCreateInfo bufferCreateInfo;
                    bufferCreateInfo.physicalDevice = physicalDevice;
                    bufferCreateInfo.logicalDevice  = logicalDevice;
                    bufferCreateInfo.size           = allocationSize;
                    bufferCreateInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;

                    Vertex::initBuffer(vertexBuffer, bufferCreateInfo);

                    void* memoryLocation = logicalDevice.mapMemory(vertexBuffer.memory, 0, bufferCreateInfo.size);
                    memcpy(memoryLocation, extractedVertices.data(), bufferCreateInfo.size);

                    logicalDevice.unmapMemory(vertexBuffer.memory);
                }

                std::vector<AllocationInfo> Manager::getAllocationInfoList()
                {
                    return allocationInfoList;
                }

                void Manager::extractVerticesFromMeshList(
                    std::vector<Vertex::Base>& outVertices,
                    size_t& outAllocationSize,
                    std::vector<AllocationInfo>& outAllocationInfoList,
                    std::vector<std::vector<Vertex::Base*>>& inMeshes
                )
                {
                    for (auto mesh : inMeshes)
                    {
                        AllocationInfo allocationInfo;
                        allocationInfo.vertexCount   = mesh.size();
                        allocationInfo.firstVertex   = outVertices.size();
                        allocationInfo.instanceCount = 1;
                        allocationInfo.firstInstance = 0;

                        outAllocationInfoList.push_back(allocationInfo);

                        outAllocationSize += sizeof(*mesh[0]) * allocationInfo.vertexCount;

                        for (auto vertex : mesh)
                        {
                            outVertices.push_back(*vertex);
                        }
                    }
                }
            }
        }
    }
}