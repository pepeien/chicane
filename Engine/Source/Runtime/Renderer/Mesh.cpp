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
                    std::vector<Vertex::Base> allVertices;

                    size_t memoryAllocationSize = 0;

                    extractVertices(allVertices, memoryAllocationSize);

                    Vertex::BufferCreateInfo bufferCreateInfo;
                    bufferCreateInfo.physicalDevice = physicalDevice;
                    bufferCreateInfo.logicalDevice  = logicalDevice;
                    bufferCreateInfo.size           = memoryAllocationSize;
                    bufferCreateInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;

                    Vertex::initBuffer(vertexBuffer, bufferCreateInfo);

                    void* memoryLocation = logicalDevice.mapMemory(vertexBuffer.memory, 0, bufferCreateInfo.size);
                    memcpy(memoryLocation, allVertices.data(), bufferCreateInfo.size);

                    logicalDevice.unmapMemory(vertexBuffer.memory);
                }

                std::vector<AllocationInfo> Manager::getAllocationInfoList()
                {
                    return meshesAllocationInfo;
                }

                void Manager::extractVertices(std::vector<Vertex::Base>& outVertices, size_t& outAllocationSize)
                {
                    for (auto vertices : meshes)
                    {
                        AllocationInfo vertexMetadata;
                        vertexMetadata.vertexCount   = vertices.size();
                        vertexMetadata.firstVertex   = outVertices.size();
                        vertexMetadata.instanceCount = 1;
                        vertexMetadata.firstInstance = 0;

                        meshesAllocationInfo.push_back(vertexMetadata);

                        outAllocationSize =+ sizeof(*vertices[0]) * vertices.size();

                        for (auto vertex : vertices)
                        {
                            outVertices.push_back(*vertex);
                        }
                    }
                }
            }
        }
    }
}