#include "Manager.hpp"

namespace Engine
{
    namespace Mesh
    {
        namespace Manager
        {
            Instance::Instance(CreateInfo& inCreateInfo)
            {
                logicalDevice  = inCreateInfo.logicalDevice;
                physicalDevice = inCreateInfo.physicalDevice;
                queue          = inCreateInfo.queue;
                commandBuffer  = inCreateInfo.commandBuffer;
            }

            Instance::~Instance()
            {
                Vertex::destroyBuffer(logicalDevice, vertexBuffer);
            }

            void Instance::addMesh(const std::string& inMeshId, const std::vector<Vertex::Base*>& inVertices)
            {
                if (meshesMap.find(inMeshId) != meshesMap.end())
                {
                    throw std::runtime_error("A Mesh identified by [" + inMeshId  + "] already exists");
                }

                Mesh::Instance newMesh;
                newMesh.vertices = inVertices;

                meshesOrder.push_back(inMeshId);
                meshesMap.insert(std::make_pair(inMeshId, newMesh));
            }

            Mesh::Instance Instance::getMesh(const std::string& inMeshId)
            {
                auto foundMesh = meshesMap.find(inMeshId);

                if (foundMesh == meshesMap.end())
                {
                    throw std::runtime_error("Invalid Mesh ID [" + inMeshId + "]");
                }

                return foundMesh->second;
            }

            void Instance::proccess()
            {
                vk::DeviceSize allocationSize = 0;

                std::vector<Vertex::Base> extractedVertices;

                extractAllocationDataFromMeshList(extractedVertices, allocationSize);

                Vertex::BufferCreateInfo stagingBufferCreateInfo;
                stagingBufferCreateInfo.physicalDevice   = physicalDevice;
                stagingBufferCreateInfo.logicalDevice    = logicalDevice;
                stagingBufferCreateInfo.size             = allocationSize;
                stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
                stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

                Vertex::Buffer stagingBuffer;
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

                Vertex::destroyBuffer(logicalDevice, stagingBuffer);
            }

            void Instance::extractAllocationDataFromMeshList(std::vector<Vertex::Base>& outVertices, vk::DeviceSize& outAllocationSize)
            {
                uint32_t currentInstance = 0;

                for (std::string& meshId : meshesOrder)
                {
                    auto& meshInstance = meshesMap.find(meshId)->second;
    
                    AllocationInfo allocationInfo;
                    allocationInfo.vertexCount   = meshInstance.vertices.size();
                    allocationInfo.firstVertex   = outVertices.size();
                    allocationInfo.instanceCount = 1;
                    allocationInfo.firstInstance = currentInstance;

                    meshInstance.allocationInfo = allocationInfo;

                    outAllocationSize += sizeof(*meshInstance.vertices[0]) * allocationInfo.vertexCount;

                    for (auto vertex : meshInstance.vertices)
                    {
                        outVertices.push_back(*vertex);
                    }

                    currentInstance++;
                }
            }
        }
    }
}