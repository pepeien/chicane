#include "Mesh.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Mesh
            {
                Manager::Manager(ManagerCreateInfo& inCreateInfo)
                {
                    logicalDevice  = inCreateInfo.logicalDevice;
                    physicalDevice = inCreateInfo.physicalDevice;
                    queue          = inCreateInfo.queue;
                    commandBuffer  = inCreateInfo.commandBuffer;
                }

                Manager::~Manager()
                {
                    destroyBuffer(vertexBuffer);
                }

                void Manager::addMesh(const std::vector<Vertex::Base*>& inVertices)
                {
                    meshes.push_back(inVertices);
                }

                void Manager::proccess()
                {
                    vk::DeviceSize allocationSize = 0;

                    std::vector<Vertex::Base> extractedVertices;

                    extractVerticesFromMeshList(
                        extractedVertices,
                        allocationSize,
                        allocationInfoList,
                        meshes
                    );
  
                    Vertex::Buffer stagingBuffer;
                    Vertex::BufferCreateInfo stagingBufferCreateInfo;
                    stagingBufferCreateInfo.physicalDevice   = physicalDevice;
                    stagingBufferCreateInfo.logicalDevice    = logicalDevice;
                    stagingBufferCreateInfo.size             = allocationSize;
                    stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
                    stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
                    Vertex::initBuffer(stagingBuffer, stagingBufferCreateInfo);

                    memcpy(
                        logicalDevice.mapMemory(
                            stagingBuffer.memory,
                            0,
                            stagingBufferCreateInfo.size
                        ),
                        extractedVertices.data(),
                        stagingBufferCreateInfo.size
                    );
                    logicalDevice.unmapMemory(stagingBuffer.memory);

                    Vertex::BufferCreateInfo bufferCreateInfo;
                    bufferCreateInfo.physicalDevice   = physicalDevice;
                    bufferCreateInfo.logicalDevice    = logicalDevice;
                    bufferCreateInfo.size             = allocationSize;
                    bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
                    bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
                    Vertex::initBuffer(vertexBuffer, bufferCreateInfo);

                    Vertex::copyBuffer(
                        stagingBuffer,
                        vertexBuffer,
                        allocationSize,
                        queue,
                        commandBuffer
                    );

                    destroyBuffer(stagingBuffer);
                }

                std::vector<AllocationInfo> Manager::getAllocationInfoList()
                {
                    return allocationInfoList;
                }

                void Manager::extractVerticesFromMeshList(
                    std::vector<Vertex::Base>& outVertices,
                    vk::DeviceSize& outAllocationSize,
                    std::vector<AllocationInfo>& outAllocationInfoList,
                    const std::vector<std::vector<Vertex::Base*>>& inMeshes
                )
                {
                    uint32_t currentInstance = 0;

                    for (auto mesh : inMeshes)
                    {
                        AllocationInfo allocationInfo;
                        allocationInfo.vertexCount   = mesh.size();
                        allocationInfo.firstVertex   = outVertices.size();
                        allocationInfo.instanceCount = 1;
                        allocationInfo.firstInstance = currentInstance;

                        outAllocationInfoList.push_back(allocationInfo);

                        outAllocationSize += sizeof(*mesh[0]) * allocationInfo.vertexCount;

                        for (auto vertex : mesh)
                        {
                            outVertices.push_back(*vertex);
                        }

                        currentInstance++;
                    }
                }

                void Manager::destroyBuffer(const Vertex::Buffer& inBuffer)
                {
                    logicalDevice.destroyBuffer(inBuffer.instance);
                    logicalDevice.freeMemory(inBuffer.memory);
                }
            }
        }
    }
}